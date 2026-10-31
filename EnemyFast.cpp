#include "EnemyFast.h"
#include "GamesEngineeringBase.h"
#include "Vector2.h"
#include "BaseEnemy.h"
#include "SmallStructs.h"
#include "BasePlayer.h"
#include "EnemyBodyNode.h"
#include <memory>

EnemyFast::EnemyFast(GamesEngineeringBase::Window& canvas, const Vector2& pos) 
	: BaseEnemy(canvas, pos){
	scale = { 50, 50 };
	renderOrder = RenderOrder::Third;
	mass = 10;
	maxVelocity = 450;
	maxForce = 1200 * mass;
	rotationSpeed = PI / 2.f;
	contactDamage = 1.f;
	hitMovement = 4.f;
	maxHealth = 1.5f; // 2 hits to death
	curHealth = 1.5f;
	invulnerableTime = 0.2f;
	collisionRadius = 10.f;
	isFixed = true;
}

void EnemyFast::OnStart() {
	BaseEnemy::OnStart();
	// build body here
	const float stiffness = 0.75f;
	const float neckDist = 30.f;
	const float bodyDist = 20.f;
	const float neckMaxBend = PI / 12;
	const float bodyMaxBend = PI / 6;
	const float brenchMaxBend = PI / 12;
	const int nodesCount = 5;
	const ColorRGBA lineColor = { 210, 210, 210, 255 };
	const float lineThickness = 5.f;
	
	std::shared_ptr<EnemyBodyNode> nodePtrs[nodesCount];
	for (int i = 0; i < nodesCount; ++i) {
		nodePtrs[i] = std::make_shared<EnemyBodyNode>(env, globalPosition + Vector2{ -10, 0 } * i,
			this, CharacterType::Enemy, BodyNodeType::Normal);
		scene->AddObject(nodePtrs[i]);
		listTails.PushBack(nodePtrs[i]);
	}

	nodePtrs[0]->mass = 4;
	nodePtrs[1]->mass = 2;
	nodePtrs[2]->mass = 1;
	nodePtrs[3]->mass = 2;
	nodePtrs[4]->mass = 1;

	angleConstraints.PushBack(
		{ ConstraintType::DirectionAngle, this, nodePtrs[0].get(), nullptr, PI, neckMaxBend , stiffness });
	angleConstraints.PushBack(
		{ ConstraintType::Angle, this, nodePtrs[0].get(), nodePtrs[1].get(), PI * 2.f / 3.f, brenchMaxBend , stiffness });
	angleConstraints.PushBack(
		{ ConstraintType::Angle, nodePtrs[0].get(), nodePtrs[1].get(), nodePtrs[2].get(), -PI * 2.f / 3.f, bodyMaxBend , stiffness });
	angleConstraints.PushBack(
		{ ConstraintType::Angle, this, nodePtrs[0].get(), nodePtrs[3].get(), -PI * 2.f / 3.f, brenchMaxBend , stiffness });
	angleConstraints.PushBack(
		{ ConstraintType::Angle, nodePtrs[0].get(), nodePtrs[3].get(), nodePtrs[4].get(), PI * 2.f / 3.f, bodyMaxBend , stiffness });

	rotationDefs.PushBack(
		{ RotationDefType::Prev, this, nodePtrs[0].get(), nullptr });
	rotationDefs.PushBack(
		{ RotationDefType::PrevAndNext, nodePtrs[0].get(), nodePtrs[1].get(), nodePtrs[2].get() });
	rotationDefs.PushBack(
		{ RotationDefType::Prev, nodePtrs[1].get(), nodePtrs[2].get(), nullptr });
	rotationDefs.PushBack(
		{ RotationDefType::PrevAndNext, nodePtrs[0].get(), nodePtrs[3].get(), nodePtrs[4].get() });
	rotationDefs.PushBack(
		{ RotationDefType::Prev, nodePtrs[3].get(), nodePtrs[4].get(), nullptr });

	distConstraints.PushBack(
		{ ConstraintType::Distance, this, nodePtrs[0].get(), nullptr, neckDist, 0.f, stiffness });
	distConstraints.PushBack(
		{ ConstraintType::Distance, nodePtrs[0].get(), nodePtrs[1].get(), nullptr, bodyDist, 0.f, stiffness });
	distConstraints.PushBack(
		{ ConstraintType::Distance, nodePtrs[1].get(), nodePtrs[2].get(), nullptr, bodyDist, 0.f, stiffness });
	distConstraints.PushBack(
		{ ConstraintType::Distance, nodePtrs[0].get(), nodePtrs[3].get(), nullptr, bodyDist, 0.f, stiffness });
	distConstraints.PushBack(
		{ ConstraintType::Distance, nodePtrs[3].get(), nodePtrs[4].get(), nullptr, bodyDist, 0.f, stiffness });

	auto ptrLine = std::make_shared<PolyLine2D>(env);
	ptrLine->lineColor = lineColor;
	ptrLine->SetThickness(lineThickness);
	ptrLine->points.PushBack(this->globalPosition);
	ptrLine->points.PushBack(nodePtrs[0]->globalPosition);
	ptrLine->points.PushBack(nodePtrs[1]->globalPosition);
	ptrLine->points.PushBack(nodePtrs[2]->globalPosition);
	scene->AddObject(ptrLine);
	polyLine2Ds.PushBack(std::move(ptrLine));
	
	ptrLine = std::make_shared<PolyLine2D>(env);
	ptrLine->lineColor = lineColor;
	ptrLine->SetThickness(lineThickness);
	ptrLine->points.PushBack(nodePtrs[0]->globalPosition);
	ptrLine->points.PushBack(nodePtrs[3]->globalPosition);
	ptrLine->points.PushBack(nodePtrs[4]->globalPosition);
	scene->AddObject(ptrLine);
	polyLine2Ds.PushBack(std::move(ptrLine));

	List<BasePhysicalObject*> tmp;
	tmp.PushBack(this), tmp.PushBack(nodePtrs[0].get()), tmp.PushBack(nodePtrs[1].get()), tmp.PushBack(nodePtrs[2].get());
	linedNodes.PushBack(std::move(tmp));

	List<BasePhysicalObject*> tmp1;
	tmp1.PushBack(nodePtrs[0].get()), tmp1.PushBack(nodePtrs[3].get()), tmp1.PushBack(nodePtrs[4].get());
	linedNodes.PushBack(std::move(tmp1));
}

void EnemyFast::FixedUpdate(const float& dt) {
	// some logic here
	BaseEnemy::FixedUpdate(dt);
}

void EnemyFast::VisualUpdate(const float& dt) {
	BaseEnemy::VisualUpdate(dt);
}

std::string EnemyFast::GetSpritePath() {
	return "./Resources/Characters/ship_K.png";
}
