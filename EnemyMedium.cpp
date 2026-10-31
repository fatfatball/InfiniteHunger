#include "EnemyMedium.h"
#include "EnemyBodyNode.h"

EnemyMedium::EnemyMedium(GamesEngineeringBase::Window& canvas, const Vector2& pos) : BaseEnemy(canvas, pos){
	scale = { 50, 50 };
	renderOrder = RenderOrder::Third;
	mass = 20.f;
	maxVelocity = 275.f;
	maxForce = 600.f * mass;
	rotationSpeed = PI;
	contactDamage = 1.f;
	hitMovement = 6.f; 
	maxHealth = 2.5f; // 3 hits to death
	curHealth = 2.5f;
	invulnerableTime = 0.2f;
	collisionRadius = 10.f;
	isFixed = true;
}

void EnemyMedium::OnStart() {
	BaseEnemy::OnStart();
	// build body here
	const float stiffness = 0.5f;
	const float neckDist = 30.f;
	const float bodyDist = 25.f;
	const float neckMaxBend = PI / 12;
	const float bodyMaxBend = PI / 6;
	const float brenchMaxBend = PI / 12;
	const int nodesCount = 5;
	const ColorRGBA lineColor = { 210, 210, 210, 255 };
	const float lineThickness = 5.f;

	std::shared_ptr<EnemyBodyNode> nodePtrs[nodesCount];
	for (int i = 0; i < nodesCount; ++i) {
		if (i == 2 || i == 4) {
			nodePtrs[i] = std::make_shared<EnemyBodyNode>(env, globalPosition + Vector2{ -10, 0 } * i,
				this, CharacterType::Enemy, BodyNodeType::Heal);
		}
		else {
			nodePtrs[i] = std::make_shared<EnemyBodyNode>(env, globalPosition + Vector2{ -10, 0 } * i,
				this, CharacterType::Enemy, BodyNodeType::Normal);
		}
		nodePtrs[i]->mass = 10.f - i;
		scene->AddObject(nodePtrs[i]);
		listTails.PushBack(nodePtrs[i]);
	}
	angleConstraints.PushBack(
		{ ConstraintType::DirectionAngle, this, nodePtrs[0].get(), nullptr, PI, neckMaxBend , stiffness });
	angleConstraints.PushBack(
		{ ConstraintType::Angle, this, nodePtrs[0].get(), nodePtrs[1].get(), PI, bodyMaxBend , stiffness });
	angleConstraints.PushBack(
		{ ConstraintType::Angle, nodePtrs[0].get(), nodePtrs[1].get(), nodePtrs[2].get(), PI, bodyMaxBend , stiffness });
	angleConstraints.PushBack(
		{ ConstraintType::Angle, nodePtrs[1].get(), nodePtrs[2].get(), nodePtrs[3].get(), PI, bodyMaxBend , stiffness });
	angleConstraints.PushBack(
		{ ConstraintType::Angle, nodePtrs[2].get(), nodePtrs[3].get(), nodePtrs[4].get(), PI, bodyMaxBend , stiffness });

	rotationDefs.PushBack(
		{ RotationDefType::PrevAndNext, this, nodePtrs[0].get(), nodePtrs[1].get() });
	rotationDefs.PushBack(
		{ RotationDefType::PrevAndNext, nodePtrs[0].get(), nodePtrs[1].get(), nodePtrs[2].get() });
	rotationDefs.PushBack(
		{ RotationDefType::PrevAndNext, nodePtrs[1].get(), nodePtrs[2].get(), nodePtrs[3].get() });
	rotationDefs.PushBack(
		{ RotationDefType::PrevAndNext, nodePtrs[2].get(), nodePtrs[3].get(), nodePtrs[4].get() });
	rotationDefs.PushBack(
		{ RotationDefType::Prev, nodePtrs[3].get(), nodePtrs[4].get(), nullptr });

	distConstraints.PushBack(
		{ ConstraintType::Distance, this, nodePtrs[0].get(), nullptr, neckDist, 0.f, stiffness });
	distConstraints.PushBack(
		{ ConstraintType::Distance, nodePtrs[0].get(), nodePtrs[1].get(), nullptr, bodyDist, 0.f, stiffness });
	distConstraints.PushBack(
		{ ConstraintType::Distance, nodePtrs[1].get(), nodePtrs[2].get(), nullptr, bodyDist, 0.f, stiffness });
	distConstraints.PushBack(
		{ ConstraintType::Distance, nodePtrs[2].get(), nodePtrs[3].get(), nullptr, bodyDist, 0.f, stiffness });
	distConstraints.PushBack(
		{ ConstraintType::Distance, nodePtrs[3].get(), nodePtrs[4].get(), nullptr, bodyDist, 0.f, stiffness });

	auto ptrLine = std::make_shared<PolyLine2D>(env);
	ptrLine->lineColor = lineColor;
	ptrLine->SetThickness(lineThickness);
	ptrLine->points.PushBack(this->globalPosition);
	ptrLine->points.PushBack(nodePtrs[0]->globalPosition);
	ptrLine->points.PushBack(nodePtrs[1]->globalPosition);
	ptrLine->points.PushBack(nodePtrs[2]->globalPosition);
	ptrLine->points.PushBack(nodePtrs[3]->globalPosition);
	ptrLine->points.PushBack(nodePtrs[4]->globalPosition);
	scene->AddObject(ptrLine);
	polyLine2Ds.PushBack(std::move(ptrLine));

	List<BasePhysicalObject*> tmp;
	tmp.PushBack(this);
	tmp.PushBack(nodePtrs[0].get());
	tmp.PushBack(nodePtrs[1].get());
	tmp.PushBack(nodePtrs[2].get());
	tmp.PushBack(nodePtrs[3].get());
	tmp.PushBack(nodePtrs[4].get());
	linedNodes.PushBack(std::move(tmp));
}

void EnemyMedium::FixedUpdate(const float& dt) {
	BaseEnemy::FixedUpdate(dt);
}

void EnemyMedium::VisualUpdate(const float& dt) {
	BaseEnemy::VisualUpdate(dt);
}

std::string EnemyMedium::GetSpritePath()
{
	return "./Resources/Characters/ship_G.png";
}
