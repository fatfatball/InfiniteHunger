#pragma once
#include "BaseItem.h"
#include "GamesEngineeringBase.h"

class ItemLockedDoor : public BaseItem {
private:
	std::string GetSpritePath() override;

public:
	ItemLockedDoor(GamesEngineeringBase::Window& env, const Vector2& pos);
	void OnCollision(BaseGameObject* other, const Vector2& normal, const Vector2& position) override;
};

