#pragma once
#include "BaseBodyNode.h"
#include "GamesEngineeringBase.h"
#include "Vector2.h"
#include "SmallStructs.h"

class PlayerTailNode : public BaseBodyNode {
private:
	std::string GetSpritePath() const override;
	PlayerType playerType;

public:
	PlayerTailNode(GamesEngineeringBase::Window& canvas) : BaseBodyNode(canvas) {}
	PlayerTailNode(GamesEngineeringBase::Window& canvas, const Vector2& pos,
		BaseCharacter* owner, const CharacterType& type, const BodyNodeType& nodeType,
		const PlayerType& playerType) :
		BaseBodyNode(canvas, pos, owner, type, nodeType) {
		this->playerType = playerType;
	}
	void VisualUpdate(const float& dt) override {}
	void OnDestroy() override {}
};

