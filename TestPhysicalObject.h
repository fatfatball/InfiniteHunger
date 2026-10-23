#pragma once
#include "BasePhysicalObject.h"

class TestPhysicalObject : public BasePhysicalObject {
public:
	float moveSpeed;
	float moveForce;
	float rotationSpeed;

	TestPhysicalObject(GamesEngineeringBase::Window& canvas);

	void OnStart() override;
	void FixedUpdate(const float& dt) override;
	void VisualUpdate(const float& dt) override;
	void OnDestroy() override;
	ColorRGBA GetColorAt(const float& u, const float& v) const override;
	unsigned char GetAlphaAt(const float& u, const float& v) const override;
};

