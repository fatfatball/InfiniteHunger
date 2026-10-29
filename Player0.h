#pragma once
#include "BasePlayer.h"
#include "GamesEngineeringBase.h"
#include <string>
#include <memory>

// player only has 1 body line
// the health refers to the length of body line
// the body node can only be changed at the end of line, like a stack
class Player0 : public BasePlayer {
public:
	Player0(GamesEngineeringBase::Window& canvas);

	void OnStart() override;

	void FixedUpdate(const float& dt) override;

	void TakeDamage(const float& damage) override;
	void Heal(const float& val) override;

private:
	std::string GetSpritePath() override;
	std::shared_ptr<BaseBodyNode> GetBodyNode(
		GamesEngineeringBase::Window& env, const Vector2& pos) override;
};

