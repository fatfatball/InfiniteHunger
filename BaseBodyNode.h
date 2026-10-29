#pragma once
#include "BasePhysicalObject.h"
#include "BaseCharacter.h"
#include "SmallStructs.h"
#include "GamesEngineeringBase.h"
#include <string>

class BaseBodyNode : public BasePhysicalObject {
public:
	BaseCharacter* owner;
	CharacterType ownerType;

	BaseBodyNode(GamesEngineeringBase::Window& canvas) : BasePhysicalObject(canvas) {
		scale = Vector2(50, 50);
		rotation = 0.f;
		maxVelocity = 500; // slightly larger than player/enemy
		maxForce = 0; // no usage here
		renderOrder = RenderOrder::Third;
		prevState = { globalPosition, scale, rotation };
		curState = { globalPosition, scale, rotation };
		isFixed = false;
		canCollide = true;
		collisionRadius = 5;
		owner = nullptr;
		ownerType = CharacterType::None;
		renderState = curState;
	}

	BaseBodyNode(GamesEngineeringBase::Window& canvas, const Vector2& pos,
		BaseCharacter* owner, const CharacterType& type) :
		BaseBodyNode(canvas) {
		this->owner = owner;
		ownerType = type;
		globalPosition = pos;
		prevState = { globalPosition, scale, rotation };
		curState = { globalPosition, scale, rotation };
		renderState = curState;
	}

	void OnStart() override {
		sprite.load(GetSpritePath());
	}

	void FixedUpdate(const float& dt) override {
		UpdatePhysicPosition(dt);
	}

	ColorRGBA GetColorAt(const float& u, const float& v) const override {
		int x = std::clamp(u, 0.f, 1.f) * sprite.width;
		int y = std::clamp(v, 0.f, 1.f) * sprite.height;
		auto ptr = sprite.atUnchecked(x, y);
		ColorRGBA color;
		color.r = ptr[0];
		color.g = ptr[1];
		color.b = ptr[2];
		color.a = sprite.alphaAtUnchecked(x, y);
		return color;
	}

	unsigned char GetAlphaAt(const float& u, const float& v) const override {
		int x = std::clamp(u, 0.f, 1.f) * sprite.width;
		int y = std::clamp(v, 0.f, 1.f) * sprite.height;
		auto ptr = sprite.atUnchecked(x, y);
		return sprite.alphaAtUnchecked(x, y);
	}

	void TakeDamage(const float& damage) {
		if (owner) {
			owner->TakeDamage(damage);
		}
	}

protected:
	GamesEngineeringBase::Image sprite;
	virtual std::string GetSpritePath() const = 0;
};

