#pragma once
#include "BaseAttack.h"

// ranged attack
class PlayerAttack1 : public BaseAttack {
private:
    float flyingSpeed;

public:
    PlayerAttack1(GamesEngineeringBase::Window& canvas, BaseCharacter* owner, const Vector2& pos);
    PlayerAttack1(GamesEngineeringBase::Window& canvas, BaseCharacter* owner, const Vector2& pos, const Vector2& dir);
    void FixedUpdate(const float& dt) override;
    void OnCollision(BaseGameObject* other, const Vector2& normal, const Vector2& position) override;

protected:
    std::string GetSpritePath() override;
};

