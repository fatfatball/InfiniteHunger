#pragma once
#include "Vector2.h"
#include "GamesEngineeringBase.h"
#include "AABB.h"
#include "Camera.h"

class TestGameObject
{
	struct State
	{
		Vector2 globalPositionS;
		Vector2 scaleS;
		float rotationS;
	};
public:
	Vector2 globalPosition;
	Vector2 scale;
	float rotation; // in rad, 0 is facing right, pi/2 is facing up
	float moveSpeed;
	float rotationSpeed;
	GamesEngineeringBase::Window& env;

	TestGameObject(GamesEngineeringBase::Window& canvas) : env(canvas) {
		globalPosition = Vector2();
		scale = Vector2(100, 100); // a square
		rotation = 0;
		moveSpeed = 600; // global position per second
		rotationSpeed = PI; // radians per second
		OnStart();
	}

	void OnStart();
	void FixedUpdate(const float& dt);
	void VisualUpdate(const float& dt);
	ColorRGBA GetColorAt(const float& u, const float& v) const; // sample the texture

	void UpdateStates(); // to store the pos and scale into curState, and move the old curState to prevState
	void RenderSelf(const float& alpha, const Camera& Camera);
	AABB GetAABB(const float& alpha, const State& renderState) const;

private:
	State prevState, curState;

	State GetRenderState(const float& alpha) const;
};

