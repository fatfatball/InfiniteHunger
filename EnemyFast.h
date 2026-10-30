#pragma once
#include "BaseEnemy.h"
#include "GamesEngineeringBase.h"
#include "Vector2.h"
#include <string>

class EnemyFast : public BaseEnemy {
public:
	EnemyFast(GamesEngineeringBase::Window& canvas, const Vector2& pos);

	void OnStart() override;

	void FixedUpdate(const float& dt) override;

	void VisualUpdate(const float& dt) override;

	void OnDestroy() override;

	void OnCollision(BaseGameObject* other, const Vector2& normal, const Vector2& position) override;

private:
	std::string GetSpritePath() override;
};

