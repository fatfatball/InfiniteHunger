#pragma once
#include "BaseBodyNode.h"
#include "SmallStructs.h"

class EnemyBodyNode : public BaseBodyNode {
private:
	std::string GetSpritePath() const override;

public:
	EnemyBodyNode(GamesEngineeringBase::Window& canvas) : BaseBodyNode(canvas) {}
	EnemyBodyNode(GamesEngineeringBase::Window& canvas, const Vector2& pos,
		BaseCharacter* owner, const CharacterType& type, const BodyNodeType& nodeType);
	void VisualUpdate(const float& dt) override {}
	void OnDestroy() override {}

	void OnCollision(BaseGameObject* other, const Vector2& normal, const Vector2& position) override;
};

