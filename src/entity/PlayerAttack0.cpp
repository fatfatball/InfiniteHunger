#include "PlayerAttack0.h"
#include "Constants.h"
#include "Vector2.h"
#include <cmath>
#include "Math.h"

PlayerAttack0::PlayerAttack0(GamesEngineeringBase::Window& canvas, BaseCharacter* owner, const Vector2& pos)
    : BaseAttack(canvas, 1.f, 0.25f, owner) {
    collisionRadius = 30.f;
    hitMovement = 10.f;
    swingRadius = 50.f;
    swingRadiance = PI;
    globalPosition = pos;
    curState.globalPositionS = globalPosition;
    prevState.globalPositionS = globalPosition;
}

void PlayerAttack0::FixedUpdate(const float& dt) {
    // assume owner ptr is valid
    float alpha = 1.f - liftimeCounter / lifetime;
    float angle = Math::WrapAngle(owner->rotation - swingRadiance / 2.f + swingRadiance * alpha);
    rotation = angle;
    Vector2 dir = { std::cos(angle), std::sin(angle) };
    globalPosition = owner->globalPosition + dir * swingRadius;

    BaseAttack::FixedUpdate(dt);
}

std::string PlayerAttack0::GetSpritePath()
{
    return "./Resources/Characters/Attacks/weapon_sword.png";
}
