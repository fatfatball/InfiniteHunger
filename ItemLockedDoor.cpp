#include "ItemLockedDoor.h"
#include "BasePlayer.h"

std::string ItemLockedDoor::GetSpritePath() {
	return "./Resources/Items/tile_door.png";
}

ItemLockedDoor::ItemLockedDoor(GamesEngineeringBase::Window& env, const Vector2& pos) 
	: BaseItem(env, pos) {
	isStatic = true;
	scale = { 64, 64 };
	collisionRadius = 32.f;
}

void ItemLockedDoor::OnCollision(BaseGameObject * other, const Vector2 & normal, const Vector2 & position) {
	if (!isActive || isPendingDestroy) {
		return;
	}

	auto player = dynamic_cast<BasePlayer*>(other);
	if (player != nullptr && !player->isDead && player->UseKey()) {
		isActive = false;
		isPendingDestroy = true;
	}
}
