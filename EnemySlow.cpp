#include "EnemySlow.h"
#include "EnemyBodyNode.h"

EnemySlow::EnemySlow(GamesEngineeringBase::Window& canvas, const Vector2& pos) 
: BaseEnemy(canvas, pos) {
	scale = { 50, 50 };
	renderOrder = RenderOrder::Third;
	mass = 30.f;
	maxVelocity = 225.f;
	maxForce = 500.f * mass;
	rotationSpeed = PI * 3.f / 2.f;
	contactDamage = 1.f;
	hitMovement = 8.f;
	maxHealth = 3.5f; // 4 hits to death
	curHealth = 3.5f;
	invulnerableTime = 0.2f;
	collisionRadius = 11.f;
	isFixed = true;
}

void EnemySlow::OnStart() {
	BaseEnemy::OnStart();
	// build body here
	const float stiffness = 0.5f;
	const float neckDist = 30.f;
	const float bodyDist = 25.f;
	const float neckMaxBend = PI / 12;
	const float bodyMaxBend = PI / 12;
	const float brenchMaxBend = PI / 24;
	const int nodesCount = 10;
	const ColorRGBA lineColor = { 210, 210, 210, 255 };
	const float lineThickness = 5.f;

	std::shared_ptr<EnemyBodyNode> nodePtrs[nodesCount];
	for (int i = 0; i < nodesCount; ++i) {
		if (i % 3 == 2) {
			nodePtrs[i] = std::make_shared<EnemyBodyNode>(env, globalPosition + Vector2{ -10, 0 } * i,
				this, CharacterType::Enemy, BodyNodeType::Heal);
		}
		else {
			nodePtrs[i] = std::make_shared<EnemyBodyNode>(env, globalPosition + Vector2{ -10, 0 } * i,
				this, CharacterType::Enemy, BodyNodeType::Normal);
		}
		nodePtrs[i]->mass = 15.f - (i % 3 + 1) * 2;
		scene->AddObject(nodePtrs[i]);
		listTails.PushBack(nodePtrs[i]);
	}
	nodePtrs[0]->mass = 20.f;
	
	angleConstraints.PushBack(
		{ ConstraintType::DirectionAngle, this, nodePtrs[0].get(), nullptr, PI, neckMaxBend , stiffness });
	angleConstraints.PushBack(
		{ ConstraintType::Angle, this, nodePtrs[0].get(), nodePtrs[1].get(), PI, brenchMaxBend , stiffness });
	angleConstraints.PushBack(
		{ ConstraintType::Angle, nodePtrs[0].get(), nodePtrs[1].get(), nodePtrs[2].get(), PI, bodyMaxBend , stiffness });
	angleConstraints.PushBack(
		{ ConstraintType::Angle, nodePtrs[1].get(), nodePtrs[2].get(), nodePtrs[3].get(), PI, bodyMaxBend , stiffness });
	angleConstraints.PushBack(
		{ ConstraintType::Angle, this, nodePtrs[0].get(), nodePtrs[4].get(), -PI * 2.f / 3.f, brenchMaxBend , stiffness });
	angleConstraints.PushBack(
		{ ConstraintType::Angle, nodePtrs[0].get(), nodePtrs[4].get(), nodePtrs[5].get(), PI, bodyMaxBend , stiffness });
	angleConstraints.PushBack(
		{ ConstraintType::Angle, nodePtrs[4].get(), nodePtrs[5].get(), nodePtrs[6].get(), PI, bodyMaxBend , stiffness });
	angleConstraints.PushBack(
		{ ConstraintType::Angle, this, nodePtrs[0].get(), nodePtrs[7].get(), PI * 2.f / 3.f, brenchMaxBend , stiffness });
	angleConstraints.PushBack(
		{ ConstraintType::Angle, nodePtrs[0].get(), nodePtrs[7].get(), nodePtrs[8].get(), PI, bodyMaxBend , stiffness });
	angleConstraints.PushBack(
		{ ConstraintType::Angle, nodePtrs[7].get(), nodePtrs[8].get(), nodePtrs[9].get(), PI, bodyMaxBend , stiffness });

	rotationDefs.PushBack(
		{ RotationDefType::Prev, this, nodePtrs[0].get(), nullptr });
	rotationDefs.PushBack(
		{ RotationDefType::PrevAndNext, nodePtrs[0].get(), nodePtrs[1].get(), nodePtrs[2].get() });
	rotationDefs.PushBack(
		{ RotationDefType::PrevAndNext, nodePtrs[1].get(), nodePtrs[2].get(), nodePtrs[3].get() });
	rotationDefs.PushBack(
		{ RotationDefType::Prev, nodePtrs[2].get(), nodePtrs[3].get(), nullptr });
	rotationDefs.PushBack(
		{ RotationDefType::PrevAndNext, nodePtrs[0].get(), nodePtrs[4].get(), nodePtrs[5].get() });
	rotationDefs.PushBack(
		{ RotationDefType::PrevAndNext, nodePtrs[4].get(), nodePtrs[5].get(), nodePtrs[6].get() });
	rotationDefs.PushBack(
		{ RotationDefType::Prev, nodePtrs[5].get(), nodePtrs[6].get(), nullptr });
	rotationDefs.PushBack(
		{ RotationDefType::PrevAndNext, nodePtrs[0].get(), nodePtrs[7].get(), nodePtrs[8].get() });
	rotationDefs.PushBack(
		{ RotationDefType::PrevAndNext, nodePtrs[7].get(), nodePtrs[8].get(), nodePtrs[9].get() });
	rotationDefs.PushBack(
		{ RotationDefType::Prev, nodePtrs[8].get(), nodePtrs[9].get(), nullptr });

	distConstraints.PushBack(
		{ ConstraintType::Distance, this, nodePtrs[0].get(), nullptr, neckDist, 0.f, stiffness });
	distConstraints.PushBack(
		{ ConstraintType::Distance, nodePtrs[0].get(), nodePtrs[1].get(), nullptr, bodyDist, 0.f, stiffness });
	distConstraints.PushBack(
		{ ConstraintType::Distance, nodePtrs[1].get(), nodePtrs[2].get(), nullptr, bodyDist, 0.f, stiffness });
	distConstraints.PushBack(
		{ ConstraintType::Distance, nodePtrs[2].get(), nodePtrs[3].get(), nullptr, bodyDist, 0.f, stiffness });
	distConstraints.PushBack(
		{ ConstraintType::Distance, nodePtrs[0].get(), nodePtrs[4].get(), nullptr, bodyDist, 0.f, stiffness });
	distConstraints.PushBack(
		{ ConstraintType::Distance, nodePtrs[4].get(), nodePtrs[5].get(), nullptr, bodyDist, 0.f, stiffness });
	distConstraints.PushBack(
		{ ConstraintType::Distance, nodePtrs[5].get(), nodePtrs[6].get(), nullptr, bodyDist, 0.f, stiffness });
	distConstraints.PushBack(
		{ ConstraintType::Distance, nodePtrs[0].get(), nodePtrs[7].get(), nullptr, bodyDist, 0.f, stiffness });
	distConstraints.PushBack(
		{ ConstraintType::Distance, nodePtrs[7].get(), nodePtrs[8].get(), nullptr, bodyDist, 0.f, stiffness });
	distConstraints.PushBack(
		{ ConstraintType::Distance, nodePtrs[8].get(), nodePtrs[9].get(), nullptr, bodyDist, 0.f, stiffness });

	auto ptrLine = std::make_shared<PolyLine2D>(env);
	ptrLine->lineColor = lineColor;
	ptrLine->SetThickness(lineThickness);
	ptrLine->points.PushBack(this->globalPosition);
	ptrLine->points.PushBack(nodePtrs[0]->globalPosition);
	ptrLine->points.PushBack(nodePtrs[1]->globalPosition);
	ptrLine->points.PushBack(nodePtrs[2]->globalPosition);
	ptrLine->points.PushBack(nodePtrs[3]->globalPosition);
	scene->AddObject(ptrLine);
	polyLine2Ds.PushBack(std::move(ptrLine));

	ptrLine = std::make_shared<PolyLine2D>(env);
	ptrLine->lineColor = lineColor;
	ptrLine->SetThickness(lineThickness);
	ptrLine->points.PushBack(nodePtrs[0]->globalPosition);
	ptrLine->points.PushBack(nodePtrs[4]->globalPosition);
	ptrLine->points.PushBack(nodePtrs[5]->globalPosition);
	ptrLine->points.PushBack(nodePtrs[6]->globalPosition);
	scene->AddObject(ptrLine);
	polyLine2Ds.PushBack(std::move(ptrLine));

	ptrLine = std::make_shared<PolyLine2D>(env);
	ptrLine->lineColor = lineColor;
	ptrLine->SetThickness(lineThickness);
	ptrLine->points.PushBack(nodePtrs[0]->globalPosition);
	ptrLine->points.PushBack(nodePtrs[7]->globalPosition);
	ptrLine->points.PushBack(nodePtrs[8]->globalPosition);
	ptrLine->points.PushBack(nodePtrs[9]->globalPosition);
	scene->AddObject(ptrLine);
	polyLine2Ds.PushBack(std::move(ptrLine));

	List<BasePhysicalObject*> tmp;
	tmp.PushBack(this);
	tmp.PushBack(nodePtrs[0].get());
	tmp.PushBack(nodePtrs[1].get());
	tmp.PushBack(nodePtrs[2].get());
	tmp.PushBack(nodePtrs[3].get());
	linedNodes.PushBack(std::move(tmp));

	List<BasePhysicalObject*> tmp1;
	tmp1.PushBack(nodePtrs[0].get());
	tmp1.PushBack(nodePtrs[4].get());
	tmp1.PushBack(nodePtrs[5].get());
	tmp1.PushBack(nodePtrs[6].get());
	linedNodes.PushBack(std::move(tmp1));
	
	List<BasePhysicalObject*> tmp2;
	tmp2.PushBack(nodePtrs[0].get());
	tmp2.PushBack(nodePtrs[7].get());
	tmp2.PushBack(nodePtrs[8].get());
	tmp2.PushBack(nodePtrs[9].get());
	linedNodes.PushBack(std::move(tmp2));
}

void EnemySlow::FixedUpdate(const float& dt) {
	BaseEnemy::FixedUpdate(dt);
}

void EnemySlow::VisualUpdate(const float& dt) {
	BaseEnemy::VisualUpdate(dt);
}

std::string EnemySlow::GetSpritePath()
{
	return "./Resources/Characters/ship_sidesA.png";
}
