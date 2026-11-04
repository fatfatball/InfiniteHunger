#pragma once
#include "BaseAttack.h"
#include "SmallStructs.h"

// magical attack
class PlayerAttack2 : public BaseAttack {
private:
    float castRange;

public:
    PlayerAttack2(GamesEngineeringBase::Window& canvas, BaseCharacter* owner, const Vector2& pos);
    void FixedUpdate(const float& dt) override;
    void OnCollision(BaseGameObject* other, const Vector2& normal, const Vector2& position) override;
    ColorRGBA GetColorAt(const float& u, const float& v) const override;
    unsigned char GetAlphaAt(const float& u, const float& v) const override;

protected:
    std::string GetSpritePath() override;
};