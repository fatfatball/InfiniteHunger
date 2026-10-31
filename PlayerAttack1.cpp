#include "PlayerAttack1.h"
#include "Vector2.h"
#include <cmath>
#include "BaseEnemy.h"

PlayerAttack1::PlayerAttack1(GamesEngineeringBase::Window& canvas,
	BaseCharacter* owner, const Vector2& pos) 
    : PlayerAttack1(canvas, owner, pos, Vector2{ std::cos(owner->rotation), std::sin(owner->rotation) }) {

}

PlayerAttack1::PlayerAttack1(GamesEngineeringBase::Window& canvas, BaseCharacter* owner,
    const Vector2& pos, const Vector2& dir) : BaseAttack(canvas, 1.f, 3.f, owner) {
    scale = { 50, 50 };
    flyingSpeed = 800.f;
    collisionRadius = 20.f;
    hitMovement = 2.f;
    globalPosition = pos + dir * 40;
    rotation = std::atan2(dir.y, dir.x);
    curState = { globalPosition, scale, rotation };
    prevState = curState;
}

void PlayerAttack1::FixedUpdate(const float& dt) {
	globalPosition = globalPosition + Vector2{std::cos(rotation), std::sin(rotation)} * flyingSpeed * dt;
	BaseAttack::FixedUpdate(dt);
}

void PlayerAttack1::OnCollision(BaseGameObject* other, const Vector2& normal, const Vector2& position) {
    if (!other->isTrigger) {
        isPendingDestroy = true;
        isActive = false;
    }
    auto ptr1 = dynamic_cast<BaseEnemy*>(other);
    if (ptr1) {
        if (ptr1->TakeDamage(damage)) {
            ptr1->globalPosition = ptr1->globalPosition +
                (ptr1->globalPosition - owner->globalPosition).Normalized() * hitMovement;
        }
        isPendingDestroy = true;
        isActive = false;
        return;
    }

    auto ptr2 = dynamic_cast<BaseBodyNode*>(other);
    if (ptr2 && ptr2->owner && ptr2->ownerType == CharacterType::Enemy) {
        if (ptr2->owner->TakeDamage(damage)) {
            ptr2->owner->globalPosition = ptr2->owner->globalPosition +
                (ptr2->owner->globalPosition - owner->globalPosition).Normalized() * hitMovement;
        }
        isPendingDestroy = true;
        isActive = false;
        return;
    }
}

std::string PlayerAttack1::GetSpritePath() {
	return "./Resources/Characters/Attacks/weapon_arrow.png";
}
