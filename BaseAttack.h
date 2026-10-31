#pragma once
#include "BasePhysicalObject.h"
#include "BaseCharacter.h"
#include "SmallStructs.h"

// the attack object class for player to cast
class BaseAttack : public BasePhysicalObject {
protected:
	float damage;
	float lifetime;
	float liftimeCounter;
    BaseCharacter* owner;
    GamesEngineeringBase::Image sprite;
    float hitMovement;

    virtual std::string GetSpritePath() = 0;

public:
    BaseAttack(GamesEngineeringBase::Window& canvas, const float& damage, const float& lifetime, BaseCharacter* owner);

    void OnStart() override;
    void FixedUpdate(const float& dt) override;
    void VisualUpdate(const float& dt) override {}
    void OnDestroy() override {}

    void TickLifetime(const float& dt);
    ColorRGBA GetColorAt(const float& u, const float& v) const override;
    unsigned char GetAlphaAt(const float& u, const float& v) const override;
    void OnCollision(BaseGameObject* other, const Vector2& normal, const Vector2& position) override;
};