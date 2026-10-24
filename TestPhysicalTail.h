#pragma once
#include "BasePhysicalObject.h"

// spine node that follows TestPhysicalObject or another TestPhyscialTail
class TestPhysicalTail : public BasePhysicalObject {
public:
	TestPhysicalTail(GamesEngineeringBase::Window& canvas);
	TestPhysicalTail(GamesEngineeringBase::Window& canvas, const Vector2& pos);

	void OnStart() override;
	void FixedUpdate(const float& dt) override;
	void VisualUpdate(const float& dt) override;
	void OnDestroy() override;
	ColorRGBA GetColorAt(const float& u, const float& v) const override;
	unsigned char GetAlphaAt(const float& u, const float& v) const override;
};

