#pragma once
#include "BasePhysicalObject.h"
#include "GamesEngineeringBase.h"
#include "Vector2.h"

class Player0TailNode : public BasePhysicalObject {
private:
	GamesEngineeringBase::Image sprite;

public:
	Player0TailNode(GamesEngineeringBase::Window& canvas);
	Player0TailNode(GamesEngineeringBase::Window& canvas, const Vector2& pos);

	void OnStart() override;
	void FixedUpdate(const float& dt) override;
	void VisualUpdate(const float& dt) override;
	void OnDestroy() override;
	ColorRGBA GetColorAt(const float& u, const float& v) const override;
	unsigned char GetAlphaAt(const float& u, const float& v) const override;
};

