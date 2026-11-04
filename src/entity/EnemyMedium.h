#pragma once
#include "BaseEnemy.h"
#include "GamesEngineeringBase.h"
#include <string>
#include "Vector2.h"

class EnemyMedium : public BaseEnemy {
public:
	EnemyMedium(GamesEngineeringBase::Window& canvas, const Vector2& pos);

	void OnStart() override;

	void FixedUpdate(const float& dt) override;

	void VisualUpdate(const float& dt) override;

private:
	std::string GetSpritePath() override;
};



