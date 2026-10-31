#include "PlayerSkill0.h"

PlayerSkill0::PlayerSkill0(BasePlayer* owner) : BaseSkill(owner) {
	durationTime = 7.5f;
}

void PlayerSkill0::CastSkill() {
	owner->SetInvulnerable(durationTime);
}
