#include "TestPhysicalObject.h"
#include "Scene.h"
#include "Math.h"
#include <memory>

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
	canCollide = true;
	collisionRadius = 35;
}

void TestPhysicalObject::OnStart() {
	// test, add 4 body nodes manually
#pragma region TempTest
	float stiffness = 0.5f;
	Vector2 startPos = { 0, 0 };
	auto ptr = std::make_shared<TestPhysicalTail>(env, startPos);
	startPos.x = -100;
	auto ptr1 = std::make_shared<TestPhysicalTail>(env, startPos);
	startPos.x = -200;
	auto ptr2 = std::make_shared<TestPhysicalTail>(env, startPos);
	startPos.x = -300;
	auto ptr3 = std::make_shared<TestPhysicalTail>(env, startPos);
	startPos.x = 0;
	startPos.y = 100;
	auto ptr4 = std::make_shared<TestPhysicalTail>(env, startPos);
	startPos.x = -100;
	startPos.y = 100;
	auto ptr5 = std::make_shared<TestPhysicalTail>(env, startPos);
	listTails.PushBack(ptr);
	listTails.PushBack(ptr1);
	listTails.PushBack(ptr2);
	listTails.PushBack(ptr3);
	listTails.PushBack(ptr4);
	listTails.PushBack(ptr5);
	scene->AddObject(ptr);
	scene->AddObject(ptr1);
	scene->AddObject(ptr2);
	scene->AddObject(ptr3);
	scene->AddObject(ptr4);
	scene->AddObject(ptr5);
	constraints.PushBack({ ConstraintType::Distance, this, ptr.get(), nullptr, 100.f, 0.f, stiffness });
	constraints.PushBack({ ConstraintType::Distance, ptr.get(), ptr1.get(), nullptr, 100.f, 0.f, stiffness });
	constraints.PushBack({ ConstraintType::Distance, ptr1.get(), ptr2.get(), nullptr, 100.f, 0.f, stiffness });
	constraints.PushBack({ ConstraintType::Distance, ptr2.get(), ptr3.get(), nullptr, 100.f, 0.f, stiffness });
	constraints.PushBack({ ConstraintType::Distance, ptr.get(), ptr4.get(), nullptr, 100.f, 0.f, stiffness });
	constraints.PushBack({ ConstraintType::Distance, ptr4.get(), ptr5.get(), nullptr, 100.f, 0.f, stiffness });

	auto ptrLine = std::make_shared<PolyLine2D>(env);
	ptrLine->lineColor = { 127, 127, 127, 255};
	ptrLine->SetThickness(20);
	ptrLine->points.Resize(5);
	polyLine2Ds.PushBack(ptrLine);
	scene->AddObject(ptrLine);
	List<BasePhysicalObject*> temp;
	temp.PushBack(this), temp.PushBack(ptr.get()), temp.PushBack(ptr1.get()), temp.PushBack(ptr2.get()), temp.PushBack(ptr3.get());
	linedNodes.PushBack(std::move(temp));
	
	ptrLine = std::make_shared<PolyLine2D>(env);
	ptrLine->lineColor = { 127, 127, 127, 255 };
	ptrLine->SetThickness(20);
	ptrLine->points.Resize(3);
	polyLine2Ds.PushBack(ptrLine);
	scene->AddObject(ptrLine);
	temp = List<BasePhysicalObject*>();
	temp.PushBack(ptr.get()), temp.PushBack(ptr4.get()), temp.PushBack(ptr5.get());
	linedNodes.PushBack(std::move(temp));

	constraints.PushBack({ ConstraintType::Angle, this, ptr.get(), ptr1.get(), PI, PI / 6.f, stiffness });
	constraints.PushBack({ ConstraintType::Angle, ptr.get(), ptr1.get(), ptr2.get(), PI, PI / 6.f, stiffness });
	constraints.PushBack({ ConstraintType::Angle, ptr1.get(), ptr2.get(), ptr3.get(), PI, PI / 6.f, stiffness });
	constraints.PushBack({ ConstraintType::Angle, this, ptr.get(), ptr4.get(), PI * 2 / 3, PI / 6.f, stiffness });
	constraints.PushBack({ ConstraintType::Angle, ptr.get(), ptr4.get(), ptr5.get(),PI, PI / 6.f, stiffness });

	constraints.PushBack({ConstraintType::DirectionAngle, this, ptr.get(), nullptr, PI, PI / 12.f , stiffness });

	rotationDefs.PushBack({ RotationDefType::PrevAndNext, this, ptr.get(), ptr1.get() });
	rotationDefs.PushBack({ RotationDefType::PrevAndNext, ptr.get(), ptr1.get(), ptr2.get() });
	rotationDefs.PushBack({ RotationDefType::PrevAndNext, ptr1.get(), ptr2.get(), ptr3.get() });
	rotationDefs.PushBack({ RotationDefType::Prev, ptr2.get(), ptr3.get(), nullptr });
	rotationDefs.PushBack({ RotationDefType::PrevAndNext, ptr.get(), ptr4.get(), ptr5.get()});
	rotationDefs.PushBack({ RotationDefType::Prev, ptr4.get(), ptr5.get(), nullptr });
#pragma endregion
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

	if (env.keyPressed(0x20)) { // space
		isPendingDestroy = true;
	}
#pragma endregion

	// the force calculation algorithm is still naive
	// TODO: make it more natural, use Steering Behaviors
	force = Vector2(std::cos(rotation), std::sin(rotation)) * maxForce * -inputDir.y;
	rotation = Math::WrapAngle(rotation + inputDir.x * rotationSpeed * dt);
	// TODO: collision should be solved here
	UpdatePhysicPosition(dt);
}

void TestPhysicalObject::SolveConstraints() {
	UpdateConstraints();
}

void TestPhysicalObject::VisualUpdate(const float& dt) {
	// update the polyline2D
	for (int i = 0; i < linedNodes.GetSize(); ++i) {
		for (int j = 0; j < linedNodes[i].GetSize(); ++j) {
			polyLine2Ds[i]->points[j] = linedNodes[i][j]->renderState.globalPositionS;
		}
	}

	// update the rotation of each body node
	Vector2 dir;
	float angle;
	for (int i = 0; i < rotationDefs.GetSize(); ++i) {
		switch (rotationDefs[i].type) {
		case RotationDefType::PrevAndNext:
			dir = rotationDefs[i].o1->globalPosition - rotationDefs[i].o3->globalPosition;
			if (dir.Length() < 1e-4f) {
				break;
			}
			angle = std::atan2(dir.y, dir.x);
			rotationDefs[i].o2->SetRotation(angle);
			break;
		case RotationDefType::Prev:
			dir = rotationDefs[i].o1->globalPosition - rotationDefs[i].o2->globalPosition;
			if (dir.Length() < 1e-4f) {
				break;
			}
			angle = std::atan2(dir.y, dir.x);
			rotationDefs[i].o2->SetRotation(angle);
			break;
		default:
			break;
		}
	}
}

void TestPhysicalObject::OnDestroy() {
	for (int i = 0; i < listTails.GetSize(); ++i) {
		listTails[i]->isPendingDestroy = true;
	}
	for (int i = 0; i < polyLine2Ds.GetSize(); ++i) {
		polyLine2Ds[i]->isPendingDestroy = true;
	}
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
		case ConstraintType::Angle:
			UpdateAngleConstraint(constraints[i].o1, constraints[i].o2, constraints[i].o3,
				constraints[i].constraint1, constraints[i].constraint2, constraints[i].stiffness);
			break;
		case ConstraintType::DirectionAngle:
			UpdateDirectionAngleConstraint(constraints[i].o1, constraints[i].o2, 
				constraints[i].constraint1, constraints[i].constraint2, constraints[i].stiffness);
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
	if (dist < 1e-4f) { // that's an error, just wait for the objects to split up
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
		if (o1->mass < 1e-4f || o2->mass < 1e-4f) {
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

void TestPhysicalObject::UpdateAngleConstraint(BasePhysicalObject* o1, BasePhysicalObject* o2,
	BasePhysicalObject* o3, const float& restAngle, const float& maxBend, const float& stiffness) {
	Vector2 v1 = o1->globalPosition - o2->globalPosition;
	Vector2 v2 = o3->globalPosition - o2->globalPosition;

	float l1 = v1.Length();
	float l2 = v2.Length();
	if (l1 < 1e-4f || l2 < 1e-4f) { // that's also an error
		return;
	}

	// calculate the angle from v1 to v2, [-pi, pi]
	float angle = std::atan2(v1.CrossProduct(v2), v1.DotProduct(v2));
	float delta = Math::AngleDifference(restAngle, angle); // [-pi, pi]

	if (delta >= -maxBend && delta <= maxBend) {
		return;
	}

	float targetDelta = std::clamp(delta, -maxBend, maxBend);
	float errAngle = delta - targetDelta; // [-pi, pi]

	// calculate grads so that points moves respectively
	Vector2 gradA = Vector2(v1.y, -v1.x) / (l1 * l1);
	Vector2 gradC = Vector2(-v2.y, v2.x) / (l2 * l2);
	Vector2 gradB = Vector2(
		-gradA.x - gradC.x,
		-gradA.y - gradC.y
	);

	// use mass as weight, please don't be 0
	float w1 = o1->isFixed ? 0.f : 1.f / o1->mass;
	float w2 = o2->isFixed ? 0.f : 1.f / o2->mass;
	float w3 = o3->isFixed ? 0.f : 1.f / o3->mass;

	float denom =
		w1 * (gradA.x * gradA.x + gradA.y * gradA.y) +
		w2 * (gradB.x * gradB.x + gradB.y * gradB.y) +
		w3 * (gradC.x * gradC.x + gradC.y * gradC.y);

	if (denom < 1e-4f) {
		return;
	}

	float lambda = stiffness * errAngle / denom;

	o1->globalPosition = o1->globalPosition - gradA * (lambda * w1);
	o2->globalPosition = o2->globalPosition - gradB * (lambda * w2);
	o3->globalPosition = o3->globalPosition - gradC * (lambda * w3);
}

void TestPhysicalObject::UpdateDirectionAngleConstraint(BasePhysicalObject* head,
	BasePhysicalObject* neckNode, const float& restAngle, const float& maxBend, const float& stiffness) {
	Vector2 v1 = { std::cos(head->rotation), std::sin(head->rotation) };
	Vector2 v2 = neckNode->globalPosition - head->globalPosition;

	float l2 = v2.Length();
	if (l2 < 1e-4f) { // that's also an error
		return;
	}

	// calculate the angle from v1 to v2, [-pi, pi]
	float angle = std::atan2(v1.CrossProduct(v2), v1.DotProduct(v2));
	float delta = Math::AngleDifference(restAngle, angle); // [-pi, pi]

	if (delta >= -maxBend && delta <= maxBend) {
		return;
	}

	float targetDelta = std::clamp(delta, -maxBend, maxBend);
	float errAngle = delta - targetDelta; // [-pi, pi]

	// regard head as fixed, move neckNode
	float a = -errAngle * stiffness;
	Vector2 rotatedV2 = {
		std::cos(a) * v2.x - std::sin(a) * v2.y,
		std::cos(a) * v2.y + std::sin(a)* v2.x
	};
	neckNode->globalPosition = head->globalPosition + rotatedV2;
}

