#pragma once
#include "BaseAttack.h"
#include <string>

class PlayerAttack0 : public BaseAttack {
private:
    float swingRadius;
    float swingRadiance;

public:
    PlayerAttack0(GamesEngineeringBase::Window& canvas, BaseCharacter* owner, const Vector2& pos);
    void FixedUpdate(const float& dt) override;

protected:
    std::string GetSpritePath() override;
};