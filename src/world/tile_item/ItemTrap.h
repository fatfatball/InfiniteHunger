#pragma once
#include "BaseItem.h"
#include "Vector2.h"

class ItemTrap : public BaseItem {
private:
	std::string GetSpritePath() override;
	float damage;
	float hitMovement;
	Vector2 projDir;
	float attackCooldown;
	float attackCooldownCounter;

public:
	ItemTrap(GamesEngineeringBase::Window& env, const Vector2& pos);

	void FixedUpdate(const float& dt) override;

	void OnCollision(BaseGameObject* other, const Vector2& normal, const Vector2& position) override;
	void SetProjectileDirection(const Vector2& dir);
	void OnTriggered();
	bool Attack();
	void TickAttackCooldown(const float& dt);
};

