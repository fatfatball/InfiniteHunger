#pragma once
#include "BaseCharacter.h"
#include "Scene.h"
#include "BaseBodyNode.h"
#include <memory>
#include "GamesEngineeringBase.h"
#include "BaseSkill.h"

class BasePlayer : public BaseCharacter {
protected:
	float physicalStiffness;
	float attackCooldown;
	float attackCooldownCounter;
	bool isFirstPlayer; // indicate that if this is the 1p, or 2p
	bool skillPressed;
	std::unique_ptr<BaseSkill> skill;
	friend class BaseSkill;
	int keyCount; // the number of keys the player has

public:
	float selfDamageTime; // as time goes by, the players get damaged. the interval shortens.
	float selfDamageTimeCounter;

	BasePlayer(GamesEngineeringBase::Window& canvas, const bool& isFirst);

	void OnStart() override;

	void FixedUpdate(const float& dt) override;

	ColorRGBA GetColorAt(const float& u, const float& v) const override;

	unsigned char GetAlphaAt(const float& u, const float& v) const override;

	bool TakeDamage(const float& damage) override;

	bool Heal(const float& val) override;

	void OnDie() override;

	virtual bool Attack();

	void TickAttackCooldown(const float& dt);

	void SetInvulnerable(const float& time);

	void AddKey();

	bool UseKey();

protected:
	void Add1BodyNode();

	void Remove1BodyNode();

	virtual std::shared_ptr<BaseBodyNode> GetBodyNode(GamesEngineeringBase::Window& env, const Vector2& pos) = 0;

	void CastSkill();

	bool TakeDamageNoInvulnerable(const float& damage);
	void TickSelfDamage(const float& dt);
};