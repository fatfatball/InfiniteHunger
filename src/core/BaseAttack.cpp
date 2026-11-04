#include "BaseAttack.h"
#include "BaseCharacter.h"
#include "BaseEnemy.h"
#include "BaseBodyNode.h"

BaseAttack::BaseAttack(GamesEngineeringBase::Window& canvas, const float& damage, const float& lifetime, BaseCharacter* owner)
    : BasePhysicalObject(canvas), damage(damage), lifetime(lifetime), owner(owner) {
    scale = { 50, 50 };
    canCollide = true;
    isTrigger = true;
    isFixed = true;
    liftimeCounter = lifetime;
    hitMovement = 10.f;
    maxVelocity = 1000.f;
    curState.scaleS = scale;
    prevState.scaleS = scale;
}

void BaseAttack::OnStart() {
    sprite.load(GetSpritePath());
}

void BaseAttack::FixedUpdate(const float& dt) {
    TickLifetime(dt);
}

void BaseAttack::TickLifetime(const float& dt) {
    liftimeCounter -= dt;
    if (liftimeCounter <= 0.f) {
        isPendingDestroy = true;
        isActive = false;
    }
}

ColorRGBA BaseAttack::GetColorAt(const float& u, const float& v) const {
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

unsigned char BaseAttack::GetAlphaAt(const float& u, const float& v) const {
    int x = std::clamp(u, 0.f, 1.f) * sprite.width;
    int y = std::clamp(v, 0.f, 1.f) * sprite.height;
    x = x == sprite.width ? x - 1 : x;
    y = y == sprite.height ? y - 1 : y;
    return sprite.alphaAtUnchecked(x, y);
}

void BaseAttack::OnCollision(BaseGameObject* other, const Vector2& normal, const Vector2& position) {
    auto ptr1 = dynamic_cast<BaseEnemy*>(other);
    if (ptr1) {
        if (ptr1->TakeDamage(damage) && !ptr1->isStatic && !ptr1->isTrigger) {
            ptr1->globalPosition = ptr1->globalPosition +
                (ptr1->globalPosition - owner->globalPosition).Normalized() * hitMovement;
        }
        return;
    }

    auto ptr2 = dynamic_cast<BaseBodyNode*>(other);
    if (ptr2 && ptr2->owner && ptr2->ownerType == CharacterType::Enemy) {
        if (ptr2->owner->TakeDamage(damage) && !ptr2->owner->isStatic && !ptr2->owner->isTrigger) {
            ptr2->owner->globalPosition = ptr2->owner->globalPosition +
                (ptr2->owner->globalPosition - owner->globalPosition).Normalized() * hitMovement;
        }
        return;
    }
}

void BaseAttack::SetDamage(const float& val) {
    damage = val;
}
