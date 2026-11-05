#pragma once
#include "BasePhysicalObject.h"
#include "SmallStructs.h"
#include "Vector2.h"
#include <string>

// only used by enemies and traps. player uses PlayerAttack
class Projectile : public BasePhysicalObject {
protected:
	float damage;
	float lifetime;
	float lifetimeCounter;
	GamesEngineeringBase::Image sprite;
	float hitMovement;
	CharacterType ownerType;
	float flyingSpeed;

public:
	Projectile(GamesEngineeringBase::Window& canvas, const Vector2& pos, const Vector2& dir, const CharacterType owner);

	void OnStart() override;
	void FixedUpdate(const float& dt) override;
	void VisualUpdate(const float& dt) override {}
	void OnDestroy() override {}

	ColorRGBA GetColorAt(const float& u, const float& v) const override;
	unsigned char GetAlphaAt(const float& u, const float& v) const override;
	void OnCollision(BaseGameObject* other, const Vector2& normal, const Vector2& position) override;

	void SetDamage(const float& val);

protected:
	std::string GetSpritePath();
	void TickLifetime(const float& dt);
};

