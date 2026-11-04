#pragma once
#include "BaseSkill.h"
#include "PlayerAttack1.h"

class PlayerSkill1 : public BaseSkill {
private:
	float durationTime;
	float durationTimeCounter;
	float initialMaxVelocity;

public:
	PlayerSkill1(BasePlayer* owner);
	void CastSkill() override;
	void Tick(const float& dt) override;
};

