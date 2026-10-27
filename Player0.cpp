#include "Player0.h"
#include "Scene.h"
#include "Player0TailNode.h"
#include "PolyLine2D.h"
#include "Vector2.h"

Player0::Player0(GamesEngineeringBase::Window& canvas) : BaseCharacter(canvas){
	maxHealth = 100.f;
	curHealth = maxHealth;
	scale = Vector2(50, 50);
	globalPosition = { 0, 0 };
	rotation = 0.01f;
	maxVelocity = 400; // global position per second
	maxForce = 1000; // global position per second^2
	rotationSpeed = PI; // radians per second
	renderOrder = RenderOrder::Third;
	prevState = { globalPosition, scale, rotation };
	curState = { globalPosition, scale, rotation };
	isFixed = true;
	canCollide = true;
	collisionRadius = 20;
	invulnerableTime = 0.3;
	invulnerableCounter = invulnerableTime;
	isInvulnerable = false;
}

void Player0::OnStart() {
	BaseCharacter::OnStart();
	scene->SetCameraTarget(this);

	float stiffness = 0.5f;
	Vector2 startPos = globalPosition - Vector2{ 35, 0 };
	auto ptr = std::make_shared<Player0TailNode>(env, startPos);
	listTails.PushBack(ptr);
	scene->AddObject(ptr);

	startPos = startPos - Vector2{ 30, 0 };
	auto ptr1 = std::make_shared<Player0TailNode>(env, startPos);
	listTails.PushBack(ptr1);
	scene->AddObject(ptr1);

	startPos = startPos - Vector2{ 30, 0 };
	auto ptr2 = std::make_shared<Player0TailNode>(env, startPos);
	listTails.PushBack(ptr2);
	scene->AddObject(ptr2);

	auto ptrLine = std::make_shared<PolyLine2D>(env);
	ptrLine->lineColor = { 210, 210, 210, 255 };
	ptrLine->SetThickness(5);
	ptrLine->points.Resize(4);
	polyLine2Ds.PushBack(ptrLine);
	scene->AddObject(ptrLine);
	List<BasePhysicalObject*> temp;
	temp.PushBack(this), temp.PushBack(ptr.get()), temp.PushBack(ptr1.get()), temp.PushBack(ptr2.get());
	linedNodes.PushBack(std::move(temp));

	constraints.PushBack({ ConstraintType::Distance, this, ptr.get(), nullptr, 35.f, 0.f, stiffness }); 
	constraints.PushBack({ ConstraintType::Distance, ptr.get(), ptr1.get(), nullptr, 30.f, 0.f, stiffness });
	constraints.PushBack({ ConstraintType::Distance, ptr1.get(), ptr2.get(), nullptr, 30.f, 0.f, stiffness });
	
	constraints.PushBack({ ConstraintType::DirectionAngle, this, ptr.get(), nullptr, PI, PI / 12.f , stiffness });
	
	constraints.PushBack({ ConstraintType::Angle, this, ptr.get(), ptr1.get(), PI, PI / 6.f, stiffness });
	constraints.PushBack({ ConstraintType::Angle, ptr.get(), ptr1.get(), ptr2.get(), PI, PI / 4.f, stiffness });

	rotationDefs.PushBack({ RotationDefType::PrevAndNext, this, ptr.get(), ptr1.get() });
	rotationDefs.PushBack({ RotationDefType::PrevAndNext, ptr.get(), ptr1.get(), ptr2.get() });
	rotationDefs.PushBack({ RotationDefType::Prev, ptr1.get(), ptr2.get(), nullptr });
}

void Player0::FixedUpdate(const float& dt) {
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

	if (env.keyPressed(0x20)) { // space
		isPendingDestroy = true;
	}

	// the force calculation algorithm is still naive
	force = Vector2(std::cos(rotation), std::sin(rotation)) * maxForce * -inputDir.y;
	rotation = Math::WrapAngle(rotation + inputDir.x * rotationSpeed * dt);

	BaseCharacter::FixedUpdate(dt);
}

void Player0::VisualUpdate(const float& dt) {
	
	BaseCharacter::VisualUpdate(dt);
}

std::string Player0::GetSpritePath()
{
	return "./Resources/Characters/red_character.png";
}
