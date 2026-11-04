#include "Projectile.h"
#include <cmath>
#include "BasePlayer.h"
#include "BaseBodyNode.h"
#include "BaseEnemy.h"

Projectile::Projectile(GamesEngineeringBase::Window& canvas, const Vector2& pos,
	const Vector2& dir, const CharacterType owner) : BasePhysicalObject(canvas), ownerType(owner){
	globalPosition = pos + dir * 40;
	scale = { 50, 50 };
	rotation = std::atan2(dir.y, dir.x);
	curState = { globalPosition, scale, rotation };
	renderState = prevState = curState;
	lifetime = 2.f;
	lifetimeCounter = lifetime;
	hitMovement = 2.f;
	flyingSpeed = 500.f;
	maxVelocity = flyingSpeed;
	damage = 1.f;
	isTrigger = true;
	canCollide = true;
	isFixed = true;
	renderOrder = RenderOrder::Third;
	collisionRadius = 20.f;
}

void Projectile::OnStart() {
	sprite.load(GetSpritePath());
}

void Projectile::FixedUpdate(const float& dt) {
	globalPosition = globalPosition + Vector2{ std::cos(rotation), std::sin(rotation) } * flyingSpeed * dt;
	TickLifetime(dt);
}

void Projectile::TickLifetime(const float& dt) {
	lifetimeCounter -= dt;
	if (lifetimeCounter <= 0.f) {
		isPendingDestroy = true;
		isActive = false;
	}
}

ColorRGBA Projectile::GetColorAt(const float& u, const float& v) const {
	int x = std::clamp(u, 0.f, 1.f) * sprite.width;
	int y = std::clamp(v, 0.f, 1.f) * sprite.height;
	x = x == sprite.width ? x - 1 : x;
	y = y == sprite.height ? y - 1 : y;
	auto ptr = sprite.atUnchecked(x, y);
	ColorRGBA color;
	color.r = ptr[0];
	color.g = ptr[1];
	color.b = ptr[2];
	color.a = sprite.alphaAtUnchecked(x, y);
	return color;
}

unsigned char Projectile::GetAlphaAt(const float& u, const float& v) const {
	int x = std::clamp(u, 0.f, 1.f) * sprite.width;
	int y = std::clamp(v, 0.f, 1.f) * sprite.height;
	x = x == sprite.width ? x - 1 : x;
	y = y == sprite.height ? y - 1 : y;
	return sprite.alphaAtUnchecked(x, y);
}

void Projectile::OnCollision(BaseGameObject* other, const Vector2& normal, const Vector2& position) {
	if (!other->isTrigger) {
		isPendingDestroy = true;
		isActive = false;
	}

	// the player's projectile is in PlayerAttack1, so here only considers None and Enemy.
	// None would damage both player and enemy, while Enemy only damages player
	if (ownerType == CharacterType::None) {
		auto ptr1 = dynamic_cast<BaseEnemy*>(other);
		if (ptr1) {
			if (ptr1->TakeDamage(damage) && !ptr1->isStatic && !ptr1->isTrigger) {
				ptr1->globalPosition = ptr1->globalPosition - normal * hitMovement;
			}
			isPendingDestroy = true;
			isActive = false;
			return;
		}

		auto ptr2 = dynamic_cast<BaseBodyNode*>(other);
		if (ptr2 && ptr2->owner && ptr2->ownerType == CharacterType::Enemy) {
			if (ptr2->owner->TakeDamage(damage) && !ptr2->owner->isStatic && !ptr2->owner->isTrigger) {
				ptr2->owner->globalPosition = ptr2->owner->globalPosition - normal * hitMovement;
			}
			isPendingDestroy = true;
			isActive = false;
			return;
		}
	}

	auto ptr3 = dynamic_cast<BasePlayer*>(other);
	if (ptr3) {
		if (ptr3->TakeDamage(damage)) {
			ptr3->globalPosition = ptr3->globalPosition - normal * hitMovement;
		}
		isPendingDestroy = true;
		isActive = false;
		return;
	}

	auto ptr4 = dynamic_cast<BaseBodyNode*>(other);
	if (ptr4 && ptr4->ownerType == CharacterType::Player) {
		if (ptr4->owner->TakeDamage(damage)) {
			ptr4->owner->globalPosition = ptr4->owner->globalPosition - normal * hitMovement;
		}
		isPendingDestroy = true;
		isActive = false;
		return;
	}
}

void Projectile::SetDamage(const float& val) {
	damage = val;
}

std::string Projectile::GetSpritePath() {
	return "./Resources/Characters/Attacks/weapon_arrow.png";
}
