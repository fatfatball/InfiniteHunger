#include "EnemyStatic.h"
#include "BaseCharacter.h"
#include "EnemyFast.h"
#include "Scene.h"

EnemyStatic::EnemyStatic(GamesEngineeringBase::Window& canvas, const Vector2& pos) 
	: BaseEnemy(canvas, pos) {
	scale = { 56, 56 };
	renderOrder = RenderOrder::Third;
	mass = 1;
	maxVelocity = 0.f;
	maxForce = 0.f;
	rotationSpeed = 0.f;
	contactDamage = 0.f;
	hitMovement = 0.f;
	maxHealth = 3.9f; // 4 hits to death
	curHealth = 3.9f;
	invulnerableTime = 0.35f;
	collisionRadius = 28.f;
	isFixed = true;
	isStatic = true;
	spawnInverval = 5.f;
	spawnInvervalCounter = spawnInverval;
	maxSpawnedEnemies = 3;
}

void EnemyStatic::OnStart() {
	BaseEnemy::OnStart();
}

void EnemyStatic::FixedUpdate(const float& dt) {
	if (chaseTarget != nullptr) {
		// re-capture the target
		chaseTarget = scene->GetClosestPlayer(globalPosition);
	}
	if (chaseTarget != nullptr) {
		
	}

	// handle triggering, if sawn by player, then set target
	else {
		force.x = force.y = 0.f;
		if (scene->GetCameraAABB().Intersects(GetAABB(curState))) {
			// find the closest player
			chaseTarget = scene->GetClosestPlayer(globalPosition);
		}
	}

	// handle despawn
	if (!scene->GetCameraAABB().Intersects(GetAABB(curState)) && !scene->IsInRoom(globalPosition)) {
		isActive = false;
		isPendingDestroy = true;
		for (int i = 0; i < listTails.GetSize(); ++i) {
			listTails[i]->isPendingDestroy = true;
		}
		for (int i = 0; i < polyLine2Ds.GetSize(); ++i) {
			polyLine2Ds[i]->isPendingDestroy = true;
		}
	}
	TickSpawn(dt);
	BaseCharacter::FixedUpdate(dt);
}

void EnemyStatic::VisualUpdate(const float& dt) {
	
}

ColorRGBA EnemyStatic::GetColorAt(const float& u, const float& v) const {
	int x = std::clamp(u, 0.f, 1.f) * sprite.width;
	int y = std::clamp(v, 0.f, 1.f) * sprite.height;
	x = x == sprite.width ? x - 1 : x;
	y = y == sprite.height ? y - 1 : y;
	auto ptr = sprite.atUnchecked(x, y);
	ColorRGBA color;
	color.r = ptr[0];
	color.g = ptr[1];
	color.b = ptr[2];
	color.a = (unsigned char)(sprite.alphaAtUnchecked(x, y) * (curHealth / maxHealth));
	return color;
}

unsigned char EnemyStatic::GetAlphaAt(const float& u, const float& v) const {
	int x = std::clamp(u, 0.f, 1.f) * sprite.width;
	int y = std::clamp(v, 0.f, 1.f) * sprite.height;
	x = x == sprite.width ? x - 1 : x;
	y = y == sprite.height ? y - 1 : y;
	return (unsigned char)(sprite.alphaAtUnchecked(x, y) * (curHealth / maxHealth));
}

std::string EnemyStatic::GetSpritePath() {
	return "./Resources/Characters/station_C.png";
}

void EnemyStatic::SpawnEnemy() {
	for (int i = spawnedEnemies.GetSize() - 1; i >= 0; --i) {
		if (spawnedEnemies[i].expired()) {
			spawnedEnemies.Erase(i);
		}
	}
	if (spawnedEnemies.GetSize() >= maxSpawnedEnemies) {
		return;
	}

	Vector2 spawnPos = globalPosition + Vector2{ 0.f, -30.f };
	std::shared_ptr<EnemyFast> spawnedEnemy = std::make_shared<EnemyFast>(env, spawnPos);
	scene->AddObject(spawnedEnemy);
	spawnedEnemies.PushBack(spawnedEnemy);
}

void EnemyStatic::TickSpawn(const float& dt) {
	if (chaseTarget == nullptr) {
		return;
	}
	spawnInvervalCounter -= dt;
	if (spawnInvervalCounter <= 0.f) {
		SpawnEnemy();
		spawnInvervalCounter = spawnInverval;
	}
}