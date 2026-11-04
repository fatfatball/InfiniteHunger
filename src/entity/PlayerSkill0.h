#pragma once
#include "BaseSkill.h"
#include "BasePlayer.h"

class PlayerSkill0 : public BaseSkill {
private:
	float durationTime;

public:
	PlayerSkill0(BasePlayer* owner);

	void CastSkill() override;
};

