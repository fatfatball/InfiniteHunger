#pragma once
#include "BaseItem.h"
#include "BaseGameObject.h"
#include "Vector2.h"

class ItemKey : public BaseItem {
private:
	std::string GetSpritePath() override;

public:
	ItemKey(GamesEngineeringBase::Window& env, const Vector2& pos);

	void OnCollision(BaseGameObject* other, const Vector2& normal, const Vector2& position) override;
};

