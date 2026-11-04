#include "ItemKey.h"
#include "BasePlayer.h"

std::string ItemKey::GetSpritePath() {
	return "./Resources/Items/tile_key.png";
}

ItemKey::ItemKey(GamesEngineeringBase::Window& env, const Vector2& pos) 
	: BaseItem(env, pos) {
	scale = { 64, 64 };
	isTrigger = true;
	collisionRadius = 25.f;
}

void ItemKey::OnCollision(BaseGameObject* other, const Vector2& normal, const Vector2& position) {
	if (!isActive || isPendingDestroy) {
		return;
	}

	auto player = dynamic_cast<BasePlayer*>(other);
	if (player != nullptr && !player->isDead) {
		isActive = false;
		isPendingDestroy = true;
		player->AddKey();
		TriggerSaving();
	}
}
