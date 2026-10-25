#include "TestPhysicalObject.h"
#include "Scene.h"
#include <iostream>

TestPhysicalObject::TestPhysicalObject(GamesEngineeringBase::Window& canvas) : BasePhysicalObject(canvas) {
	scale = Vector2(70, 70);
	globalPosition = { 100, 0 };
	rotation = 0;
	maxVelocity = 600; // global position per second
	maxForce = 2000; // global position per second^2
	rotationSpeed = PI; // radians per second
	renderOrder = RenderOrder::Third;
	prevState = { globalPosition, scale, rotation };
	curState = { globalPosition, scale, rotation };
	isFixed = true;
	polyLine2DPtr = nullptr;
}

void TestPhysicalObject::OnStart() {
	// test, add 4 body nodes manually
	auto ptr = std::make_shared<TestPhysicalTail>(env);
	Vector2 startPos = { -100, 0 };
	auto ptr1 = std::make_shared<TestPhysicalTail>(env, startPos);
	startPos.x = -200;
	auto ptr2 = std::make_shared<TestPhysicalTail>(env, startPos);
	startPos.x = -300;
	auto ptr3 = std::make_shared<TestPhysicalTail>(env, startPos);
	listTails.PushBack(ptr);
	listTails.PushBack(ptr1);
	listTails.PushBack(ptr2);
	listTails.PushBack(ptr3);
	scene->AddObject(ptr);
	scene->AddObject(ptr1);
	scene->AddObject(ptr2);
	scene->AddObject(ptr3);
	constraints.PushBack({ ConstraintType::Distance, this, ptr.get(), nullptr, 100.f, 0.f, 0.8f });
	constraints.PushBack({ ConstraintType::Distance, ptr.get(), ptr1.get(), nullptr, 100.f, 0.f, 0.8f });
	constraints.PushBack({ ConstraintType::Distance, ptr1.get(), ptr2.get(), nullptr, 100.f, 0.f, 0.8f });
	constraints.PushBack({ ConstraintType::Distance, ptr2.get(), ptr3.get(), nullptr, 100.f, 0.f, 0.8f });

	auto ptr4 = std::make_shared<PolyLine2D>(env);
	ptr4->points.PushBack(globalPosition);
	ptr4->points.PushBack(ptr->globalPosition);
	ptr4->points.PushBack(ptr1->globalPosition);
	ptr4->points.PushBack(ptr2->globalPosition);
	ptr4->points.PushBack(ptr3->globalPosition);
	ptr4->lineColor = { 127, 127, 127, 255};
	ptr4->SetThickness(25);
	polyLine2DPtr = ptr4.get();
	scene->AddObject(ptr4);
}

void TestPhysicalObject::FixedUpdate(const float& dt) {
#pragma region InputHandle
	Vector2 inputDir = Vector2();
	if (env.keyPressed(0x57)) { // W
		inputDir.y += -1;
	}
	if (env.keyPressed(0x41)) { // A
		inputDir.x += -1;
	}
	if (env.keyPressed(0x53)) { // S
		inputDir.y += 1;
	}
	if (env.keyPressed(0x44)) { // D
		inputDir.x += 1;
	}

	if (inputDir.Length() > 0) {
		inputDir = inputDir.Normalized();
	}

	float inputAngle = 0;
	if (env.keyPressed(0x51)) { // Q
		inputAngle += 1;
	}
	if (env.keyPressed(0x45)) { // E
		inputAngle -= 1;
	}

	if (env.keyPressed(0x20)) { // space
		isPendingDestroy = true;
	}
#pragma endregion

	// the force calculation algorithm is still naive
	// TODO: make it more natural, use Steering Behaviors
	force = inputDir * std::clamp(((inputDir * maxVelocity - curPhysicState.velocityS).Length() / maxVelocity), 0.f, 1.f) * maxForce;
	rotation = Math::WrapAngle(rotation + inputAngle * rotationSpeed * dt);
	// TODO: collision should be solved here
	UpdatePhysicPosition(dt);
}

void TestPhysicalObject::SolveConstraints() {
	UpdateConstraints();
}

void TestPhysicalObject::VisualUpdate(const float& dt) {
	polyLine2DPtr->points[0] = renderState.globalPositionS;
	for (int i = 0; i < listTails.GetSize(); ++i) {
		polyLine2DPtr->points[i + 1] = listTails[i]->renderState.globalPositionS;
	}
}

void TestPhysicalObject::OnDestroy() {
	int size = listTails.GetSize();
	for (int i = 0; i < size; ++i) {
		listTails[i]->isPendingDestroy = true;
	}
	std::cout << "I'm destroyed!" << std::endl;
}

ColorRGBA TestPhysicalObject::GetColorAt(const float& u, const float& v) const {
	ColorRGBA color;
	color.r = 255;
	color.g = 255;
	color.b = 255;
	color.a = GetAlphaAt(u, v);
	return color;
}

unsigned char TestPhysicalObject::GetAlphaAt(const float& u, const float& v) const
{
	// triangle facing right
	if ((v - 0.577f * u > -0.077f) && (v + 0.577f * u < 1.077f) && (u > 0.134)) {
		return 255;
	}
	else {
		return 0;
	}
}

void TestPhysicalObject::UpdateConstraints() {
	// Gauss-Seidel method, update the constraints one by one
	// here is from head to tail
	// Jacobi method will first calculate all constraints and the gradient, then move them all.
	// but it's just too complicated to imply
	int size = constraints.GetSize();
	// only iterate one time here. the more iteration, the stiffer it would be
	for (int i = 0; i < size; ++i) {
		switch (constraints[i].type) {
		case ConstraintType::Distance:
			UpdateDistanceConstraint(constraints[i].o1, constraints[i].o2,
				constraints[i].constraint1, constraints[i].stiffness);
			break;
		default:
			break;
		}
	}
}

void TestPhysicalObject::UpdateDistanceConstraint(BasePhysicalObject* o1, BasePhysicalObject* o2,
	const float& targetDist, const float& stiffness) {
	if (o1->isFixed && o2->isFixed) {
		return;
	}

	Vector2 vec = o2->globalPosition - o1->globalPosition;
	float dist = vec.Length();
	if (dist < 1e-4) { // that's an error, just wait for the objects to split up
		return;
	}
	float diff = (dist - targetDist) * stiffness;

	if (o1->isFixed) {
		o2->globalPosition = o2->globalPosition - vec.Normalized() * diff;
	}
	else if (o2->isFixed) {
		o1->globalPosition = o1->globalPosition + vec.Normalized() * diff;
	}
	else {
		if (o1->mass < 1e-4 || o2->mass < 1e-4) {
			o1->globalPosition = o1->globalPosition + vec.Normalized() * diff * 0.5f;
			o2->globalPosition = o2->globalPosition - vec.Normalized() * diff * 0.5f;
		}
		else {
			// to make sure momentum stays the same
			float w1 = 1.f / o1->mass;
			float w2 = 1.f / o2->mass;
			o1->globalPosition = o1->globalPosition + vec.Normalized() * diff * (w1 / (w1 + w2));
			o2->globalPosition = o2->globalPosition - vec.Normalized() * diff * (w2 / (w1 + w2));
		}
	}
}