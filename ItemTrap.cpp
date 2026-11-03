#include "ItemTrap.h"
#include "SmallStructs.h"
#include "BaseEnemy.h"
#include "BasePlayer.h"
#include "Projectile.h"

std::string ItemTrap::GetSpritePath() {
	return "./Resources/Items/trap.png";
}

ItemTrap::ItemTrap(GamesEngineeringBase::Window& env, const Vector2& pos) : BaseItem(env, pos) {
	scale = { 64, 64 };
	isTrigger = false;
	isStatic = true;
	collisionRadius = 20.f;
	damage = 1.0f;
	hitMovement = 8.f;
	projDir = Vector2();
	attackCooldown = 1.f;
	attackCooldownCounter = attackCooldown;
}

void ItemTrap::FixedUpdate(const float& dt) {
	BaseItem::FixedUpdate(dt);
	TickAttackCooldown(dt);
}

void ItemTrap::OnCollision(BaseGameObject* other, const Vector2& normal, const Vector2& position) {
	// hurt both player and enemy
	auto ptr1 = dynamic_cast<BaseEnemy*>(other);
	if (ptr1) {
		if (ptr1->TakeDamage(damage) && !ptr1->isStatic && !ptr1->isTrigger) {
			ptr1->globalPosition = ptr1->globalPosition - normal * hitMovement;
		}
		return;
	}

	auto ptr2 = dynamic_cast<BaseBodyNode*>(other);
	if (ptr2 && ptr2->owner && ptr2->ownerType == CharacterType::Enemy) {
		if (ptr2->owner->TakeDamage(damage) && !ptr2->owner->isStatic && !ptr2->owner->isTrigger) {
			ptr2->owner->globalPosition = ptr2->owner->globalPosition - normal * hitMovement;
		}
		return;
	}


	auto ptr3 = dynamic_cast<BasePlayer*>(other);
	if (ptr3) {
		if (ptr3->TakeDamage(damage)) {
			ptr3->globalPosition = ptr3->globalPosition - normal * hitMovement;
		}
		return;
	}

	auto ptr4 = dynamic_cast<BaseBodyNode*>(other);
	if (ptr4 && ptr4->ownerType == CharacterType::Player) {
		if (ptr4->owner->TakeDamage(damage)) {
			ptr4->owner->globalPosition = ptr4->owner->globalPosition - normal * hitMovement;
		}
		return;
	}
}

void ItemTrap::SetProjectileDirection(const Vector2& dir) {
	projDir = dir;
}

void ItemTrap::OnTriggered() {
	Attack();
}

bool ItemTrap::Attack() {
	if (attackCooldownCounter > 0) {
		return false;
	}

	auto ptr = std::make_shared<Projectile>(env, globalPosition, projDir, CharacterType::None);
	scene->AddObject(ptr);

	attackCooldownCounter = attackCooldown;
	return true;
}

void ItemTrap::TickAttackCooldown(const float& dt) {
	if (attackCooldownCounter > 0) {
		attackCooldownCounter -= dt;
	}
}
