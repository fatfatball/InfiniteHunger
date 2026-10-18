#pragma once
#include "Vector3.h"
#include "GamesEngineeringBase.h"

class TestGameObject
{
public:
	Vector3 globalPosition;
	Vector3 scale;
	float moveSpeed;
	GamesEngineeringBase::Window env;

	TestGameObject(GamesEngineeringBase::Window canvas) {
		globalPosition = Vector3();
		scale = Vector3(100, 100, 1); // a square
		moveSpeed = 300; // global position per second
		env = canvas;
		OnStart();
	}

	void OnStart();
	void FixedUpdate(float dt);
	void VisualUpdate(float dt);
	ColorRGB GetColorAt(float u, float v); // sample the texture

	void UpdateStates(); // to store the pos and scale into curState, and move the old curState to prevState

private:
	struct State
	{
		Vector3 globalPositionS;
		Vector3 scaleS;
	} prevState, curState;

	State GetRenderState(float alpha);
};

