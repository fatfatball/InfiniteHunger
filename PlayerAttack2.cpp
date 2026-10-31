#include "PlayerAttack2.h"
#include "BaseEnemy.h"

PlayerAttack2::PlayerAttack2(GamesEngineeringBase::Window& canvas,
	BaseCharacter* owner, const Vector2& pos) : BaseAttack(canvas, 1.f, 0.25f, owner) {
	scale = { 70, 70 };
	collisionRadius = 35.f;
	hitMovement = 10.f;
	castRange = 200.f;
	globalPosition = pos + Vector2{ std::cos(owner->rotation), std::sin(owner->rotation) } * castRange;
	rotation = owner->rotation;
	curState = { globalPosition, scale, rotation };
	prevState = curState;
}

void PlayerAttack2::FixedUpdate(const float& dt) {
	BaseAttack::FixedUpdate(dt);
}

void PlayerAttack2::OnCollision(BaseGameObject * other, const Vector2 & normal, const Vector2 & position) {
    auto ptr1 = dynamic_cast<BaseEnemy*>(other);
    if (ptr1) {
        if (ptr1->TakeDamage(damage)) {
            ptr1->globalPosition = ptr1->globalPosition +
                (ptr1->globalPosition - owner->globalPosition).Normalized() * hitMovement;
        }
        return;
    }

    auto ptr2 = dynamic_cast<BaseBodyNode*>(other);
    if (ptr2 && ptr2->owner && ptr2->ownerType == CharacterType::Enemy) {
        if (ptr2->owner->TakeDamage(damage)) {
            ptr2->owner->globalPosition = ptr2->owner->globalPosition +
                (ptr2->owner->globalPosition - owner->globalPosition).Normalized() * hitMovement;
        }
        return;
    }
}

ColorRGBA PlayerAttack2::GetColorAt(const float& u, const float& v) const {
    int x = std::clamp(u, 0.f, 1.f) * sprite.width;
    int y = std::clamp(v, 0.f, 1.f) * sprite.height;
    x = x == sprite.width ? x - 1 : x;
    y = y == sprite.height ? y - 1 : y;
    auto ptr = sprite.atUnchecked(x, y);
    ColorRGBA color;
    color.r = ptr[0];
    color.g = ptr[1];
    color.b = ptr[2];
    color.a = sprite.alphaAtUnchecked(x, y) * (liftimeCounter / lifetime);
    return color;
}

unsigned char PlayerAttack2::GetAlphaAt(const float& u, const float& v) const {
    int x = std::clamp(u, 0.f, 1.f) * sprite.width;
    int y = std::clamp(v, 0.f, 1.f) * sprite.height;
    x = x == sprite.width ? x - 1 : x;
    y = y == sprite.height ? y - 1 : y;
    auto res = sprite.alphaAtUnchecked(x, y);
    if (res != 0) {
        return 127;
    }
    return res;
}

std::string PlayerAttack2::GetSpritePath() {
	return "./Resources/Characters/Attacks/tree.png";
}
