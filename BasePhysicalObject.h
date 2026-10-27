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
	float dampValue; // without force input, the velocity decreases to velocity * dampValue per second
	float mass;
	float maxVelocity;
	float maxForce;
	bool isFixed; // if it can be moved by constraints (not including collision)
	float collisionRadius;

	BasePhysicalObject(GamesEngineeringBase::Window& canvas) : BaseGameObject(canvas) {
		dampValue = 0.1f;
		mass = 1;
		maxVelocity = 0;
		maxForce = 0;
		isFixed = false;
		collisionRadius = 0;
	}

	virtual void UpdatePhysicPosition(const float& dt) {
		// update position using Verlet algorithm
		// calculate velocity approximately
		velocity = curPhysicState.velocityS + force / mass * dt;
		// clamp velocity to maxVelocity
		if (velocity.Length() > maxVelocity) {
			velocity = velocity.Normalized() * maxVelocity;
		}

		// calculate damping
		float damping = std::powf(dampValue, dt);
		velocity = velocity * damping;
		// apply velocity
		globalPosition = curState.globalPositionS + velocity * dt;
	}

	void UpdateVelocity(const float& dt) override {
		velocity = (globalPosition - curState.globalPositionS) / dt;
		curPhysicState.velocityS = velocity;
	}

	void UpdateStates() override { 
		prevState = curState;
		curState.globalPositionS = globalPosition;
		curState.scaleS = scale;
		curState.rotationS = rotation;
		prevPhysicState = curPhysicState;
		curPhysicState = { velocity, force };
	}

	virtual void OnCollision(const BaseGameObject* other, const Vector2& normal, const Vector2& position) {}
};