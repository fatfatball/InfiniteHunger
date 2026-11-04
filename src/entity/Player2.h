#pragma once
#include "BasePlayer.h"
#include "GamesEngineeringBase.h"
#include <string>
#include <memory>

// wizard player
class Player2 : public BasePlayer {
public:
	Player2(GamesEngineeringBase::Window& canvas, const bool& isFirst);

	void OnStart() override;

	void FixedUpdate(const float& dt) override;

	bool Attack() override;

private:
	std::string GetSpritePath() override;
	std::shared_ptr<BaseBodyNode> GetBodyNode(
		GamesEngineeringBase::Window& env, const Vector2& pos) override;
};

