#include "PlayerSkill1.h"
#include "BasePlayer.h"
#include "PlayerAttack1.h"
#include "Vector2.h"
#include <cmath>
#include "Constants.h"

PlayerSkill1::PlayerSkill1(BasePlayer* owner) : BaseSkill(owner){
	durationTime = 10.f;
	durationTimeCounter = 0.f;
	initialMaxVelocity = owner->maxVelocity;
}

void PlayerSkill1::CastSkill() {
	owner->maxVelocity = initialMaxVelocity * 1.25f;
	durationTimeCounter = durationTime;

	// shoot arrows
	Vector2 dir = { 0.f, 0.f };
	float angle = 0.f;
	for (int i = 0; i < 16; ++i) {
		angle += PI / 8.f;
		dir = { std::cos(angle), std::sin(angle) };
		auto ptr = std::make_shared<PlayerAttack1>(owner->env, owner, owner->globalPosition, dir);
		owner->scene->AddObject(ptr);
	}
}

void PlayerSkill1::Tick(const float& dt) {
	if (durationTimeCounter > 0) {
		durationTimeCounter -= dt;
		if (durationTimeCounter <= 0) {
			owner->maxVelocity = initialMaxVelocity;
		}
	}
}
