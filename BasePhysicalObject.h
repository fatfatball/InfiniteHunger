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
	float maxVelocity;
	float maxForce;
	float isFixed; // if it can be moved by constraints

	BasePhysicalObject(GamesEngineeringBase::Window& canvas) : BaseGameObject(canvas) {
		dampValue = 0.04f;
		mass = 1;
		maxVelocity = 0;
		maxForce = 0;
		isFixed = false;
	}

	virtual void UpdatePhysicPosition(const float& dt) {
		// update position using Verlet algorithm + Position Based Dynamics
		// calculate velocity approximately
		velocity = (curState.globalPositionS - prevState.globalPositionS) / dt + force / mass * dt;
		// clamp velocity to maxVelocity
		if (velocity.Length() > maxVelocity) {
			velocity = velocity.Normalized() * maxVelocity;
		}

		// calculate damping
		float damping = std::powf(dampValue, dt);
		velocity = velocity * damping;
		// apply velocity
		globalPosition = curState.globalPositionS + velocity * dt;

		prevPhysicState = curPhysicState;
		curPhysicState = { velocity, force };
	}
};