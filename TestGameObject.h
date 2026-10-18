#pragma once
#include "Vector2.h"
#include "GamesEngineeringBase.h"
#include "AABB.h"
#include "Camera.h"

class TestGameObject
{
public:
	Vector2 globalPosition;
	Vector2 scale;
	float moveSpeed;
	GamesEngineeringBase::Window& env;

	TestGameObject(GamesEngineeringBase::Window& canvas) : env(canvas) {
		globalPosition = Vector2();
		scale = Vector2(100, 100); // a square
		moveSpeed = 600; // global position per second
		OnStart();
	}

	void OnStart();
	void FixedUpdate(const float& dt);
	void VisualUpdate(const float& dt);
	ColorRGBA GetColorAt(const float& u, const float& v) const; // sample the texture

	void UpdateStates(); // to store the pos and scale into curState, and move the old curState to prevState
	void RenderSelf(const float& alpha, const Camera& Camera);
	AABB GetAABB(const float& alpha) const;

private:
	struct State
	{
		Vector2 globalPositionS;
		Vector2 scaleS;
	} prevState, curState;

	State GetRenderState(const float& alpha) const;
};

