#pragma once
#include "Vector3.h"
#include "GamesEngineeringBase.h"
#include "AABB.h"
#include "Camera.h"

class TestGameObject
{
public:
	Vector3 globalPosition;
	Vector3 scale;
	float moveSpeed;
	GamesEngineeringBase::Window& env;

	TestGameObject(GamesEngineeringBase::Window& canvas) : env(canvas) {
		globalPosition = Vector3();
		scale = Vector3(100, 100, 1); // a square
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
		Vector3 globalPositionS;
		Vector3 scaleS;
	} prevState, curState;

	State GetRenderState(const float& alpha) const;
};

