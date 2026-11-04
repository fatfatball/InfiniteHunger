#pragma once
#include "Vector2.h"
#include "GamesEngineeringBase.h"
#include "AABB.h"
#include "Camera.h"
#include "BaseGameObject.h"
#include "SmallStructs.h"

class TestGameObject : public BaseGameObject {
public:
	float moveSpeed;
	float rotationSpeed;
	float zoomSpeed;

	TestGameObject(GamesEngineeringBase::Window& canvas);

	void OnStart() override;
	void FixedUpdate(const float& dt) override;
	void VisualUpdate(const float& dt) override;
	void OnDestroy() override;
	ColorRGBA GetColorAt(const float& u, const float& v) const override; // sample the texture
	unsigned char GetAlphaAt(const float& u, const float& v) const override;
};

