#pragma once
#include "BasePhysicalObject.h"
#include "GamesEngineeringBase.h"
#include <string>

class BaseItem : public BasePhysicalObject {
public:
    BaseItem(GamesEngineeringBase::Window& env, const Vector2& pos);

	void OnStart() override;
	void FixedUpdate(const float& dt) override {}
	void VisualUpdate(const float& dt) override {}
	void OnDestroy() override {}
	ColorRGBA GetColorAt(const float& u, const float& v) const override;
	unsigned char GetAlphaAt(const float& u, const float& v) const override;

private:
	GamesEngineeringBase::Image sprite;

	virtual std::string GetSpritePath() = 0;
};

