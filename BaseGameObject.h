#pragma once
#include "Vector2.h"
#include "GamesEngineeringBase.h"
#include "AABB.h"
#include "Camera.h"

class BaseGameObject
{
	// TODO: extract the logic in TestGameObject to this class
	// and make TestGameObject inherit from this class
public:
	bool isActive;
	bool isRenderable;

	virtual ~BaseGameObject() = default;

	virtual void OnStart() = 0;
	virtual void FixedUpdate(const float& dt) = 0;
	virtual void VisualUpdate(const float& dt) = 0;
	virtual void OnDestroy() = 0;
};

