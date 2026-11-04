#pragma once
#include "BaseSkill.h"
#include "BasePlayer.h"

class PlayerSkill2 : public BaseSkill {
public:
	PlayerSkill2(BasePlayer* owner);

	void CastSkill() override;
};

