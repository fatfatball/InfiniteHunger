#pragma once
#include "BaseAttack.h"

// ranged attack
class PlayerAttack1 : public BaseAttack {
private:
    float flyingSpeed;

public:
    PlayerAttack1(GamesEngineeringBase::Window& canvas, BaseCharacter* owner, const Vector2& pos);
    void FixedUpdate(const float& dt) override;
    void OnCollision(BaseGameObject* other, const Vector2& normal, const Vector2& position) override;

protected:
    std::string GetSpritePath() override;
};

