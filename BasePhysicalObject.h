#pragma once
#include "BaseGameObject.h"
#include "Vector2.h"
#include <cmath>

class BasePhysicalObject : public BaseGameObject {
protected:
	struct PhysicState {
		Vector2 velocityS;
		Vector2 accelerationS;
	}prevPhysicState, curPhysicState;
	Vector2 velocity;
	Vector2 force;

public:
	float dampValue; // without force input, the velocity decrease to velocity * dampValue per second
	float mass;

	BasePhysicalObject(GamesEngineeringBase::Window& canvas) : BaseGameObject(canvas) {
		dampValue = 0.75f;
		mass = 1;
	}

	void UpdatePhysicStates(const float& dt) {
		// update position using Verlet algorithm
		velocity = (curState.globalPositionS - prevState.globalPositionS) / dt;
		float damping = std::powf(dampValue, dt);
		globalPosition = curState.globalPositionS + velocity * dt * damping + force / mass * dt * dt;

		prevPhysicState = curPhysicState;
		curPhysicState = { velocity, force };
	}
};