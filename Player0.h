#pragma once
#include "BaseCharacter.h"
#include "GamesEngineeringBase.h"
#include <string>

class Player0 : public BaseCharacter {
public:
	Player0(GamesEngineeringBase::Window& canvas);

	void OnStart() override;
	void FixedUpdate(const float& dt) override;
	void VisualUpdate(const float& dt) override;

private:
	std::string GetSpritePath() override;
};

