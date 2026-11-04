#include "PlayerSkill2.h"
#include "BasePlayer.h"
#include <memory>
#include "PlayerAttack2.h"

PlayerSkill2::PlayerSkill2(BasePlayer* owner) : BaseSkill(owner){

}

void PlayerSkill2::CastSkill() {
	auto ptr = std::make_shared<PlayerAttack2>(owner->env, owner, owner->globalPosition);
	ptr->scale = { 300.f, 300.f };
	ptr->collisionRadius = 150.f;
	ptr->SetDamage(2.f);
	ptr->globalPosition = owner->globalPosition;
	ptr->ResetStates();
	owner->scene->AddObject(ptr);
}
