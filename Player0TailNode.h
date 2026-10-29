#pragma once
#include "BaseBodyNode.h"
#include "GamesEngineeringBase.h"
#include "Vector2.h"

class Player0TailNode : public BaseBodyNode {
private:
	std::string GetSpritePath() const override;

public:
	Player0TailNode(GamesEngineeringBase::Window& canvas) : BaseBodyNode(canvas) {}
	Player0TailNode(GamesEngineeringBase::Window& canvas, const Vector2& pos,
		BaseCharacter* owner, const CharacterType& type) :
		BaseBodyNode(canvas, pos, owner, type) {}
	void VisualUpdate(const float& dt) override {}
	void OnDestroy() override {}
};

