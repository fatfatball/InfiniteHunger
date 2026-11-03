#include "ItemTrapTrigger.h"
#include "BaseBodyNode.h"
#include "BaseEnemy.h"
#include "BasePlayer.h"
#include "ItemTrap.h"

std::string ItemTrapTrigger::GetSpritePath() {
	return "./Resources/Items/trapdoor_round.png";
}

ItemTrapTrigger::ItemTrapTrigger(GamesEngineeringBase::Window& env,
	const Vector2& pos, std::weak_ptr<ItemTrap> target) : BaseItem(env, pos){
	targetTrap = target;
	scale = { 64, 64 };
	isTrigger = true;
	isStatic = true;
	collisionRadius = 20.f;
}

void ItemTrapTrigger::OnCollision(BaseGameObject * other, const Vector2 & normal, const Vector2 & position) {
	// when stepped by enemy / player / bodynode, trigger the trap
	auto ptr1 = dynamic_cast<BaseEnemy*>(other);
	if (ptr1) {
		TriggerTrap();
		return;
	}

	auto ptr2 = dynamic_cast<BaseBodyNode*>(other);
	if (ptr2 && ptr2->owner) {
		TriggerTrap();
		return;
	}

	auto ptr3 = dynamic_cast<BasePlayer*>(other);
	if (ptr3) {
		TriggerTrap();
		return;
	}
}

void ItemTrapTrigger::TriggerTrap() {
	if (targetTrap.expired()) {
		return;
	}
	else {
		targetTrap.lock()->OnTriggered();
	}
}
