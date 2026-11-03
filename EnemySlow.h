#pragma once
#include "BaseEnemy.h"
#include "GamesEngineeringBase.h"
#include <string>
#include "Vector2.h"
#include "Projectile.h"

class EnemySlow : public BaseEnemy {
public:
	EnemySlow(GamesEngineeringBase::Window& canvas, const Vector2& pos);

	void OnStart() override;

	void FixedUpdate(const float& dt) override;

	void VisualUpdate(const float& dt) override;

private:
	float attackCooldown;
	float attackCooldownCounter;

	void TickAttackCooldown(const float& dt);
	bool Attack();
	std::string GetSpritePath() override;
};

