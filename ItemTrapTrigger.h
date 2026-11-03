#pragma once
#include "BaseItem.h"
#include "ItemTrap.h"
#include <memory>

class ItemTrapTrigger : public BaseItem {
private:
	std::weak_ptr<ItemTrap> targetTrap;
	std::string GetSpritePath() override;

public:
	ItemTrapTrigger(GamesEngineeringBase::Window& env, const Vector2& pos, std::weak_ptr<ItemTrap> target);

	void OnCollision(BaseGameObject* other, const Vector2& normal, const Vector2& position) override;
	void TriggerTrap();
};

