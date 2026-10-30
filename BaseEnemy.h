#pragma once
#include "BaseCharacter.h"
#include "GamesEngineeringBase.h"
#include "Vector2.h"
#include <cmath>
#include "Math.h"
#include "Scene.h"
#include "BasePlayer.h"

// base class for enemies.
// enemies spawn when the room is generated
// start to chase player when seen by camera, choose the closest one
// despawn when not inside a room (when the room is destroyed,
// but there are still enemies inside)

class BaseEnemy : public BaseCharacter {
protected:
	BaseCharacter* chaseTarget;
	float contactDamage;

public:
	BaseEnemy(GamesEngineeringBase::Window& canvas) : BaseCharacter(canvas) {
		chaseTarget = nullptr;
		contactDamage = 1.f;
		isFixed = true;
		canCollide = true;
		renderOrder = RenderOrder::Third;
		mass = 15;
		maxVelocity = 300;
		maxForce = 800 * mass;
		rotationSpeed = PI;
	}

	BaseEnemy(GamesEngineeringBase::Window& canvas, const Vector2& pos) : BaseEnemy(canvas) {
		globalPosition = pos;
		curState.globalPositionS = globalPosition;
		prevState.globalPositionS = globalPosition;
	}

	void FixedUpdate(const float& dt) override {
		// handle movement
		if (chaseTarget != nullptr) {
			Vector2 dir = chaseTarget->globalPosition - globalPosition;
			if (dir.Length() > 1e-4f) {
				float theta = std::atan2(dir.y, dir.x);
				float deltaTheta = Math::WrapAngle(theta - rotation);
				rotation = rotation + deltaTheta > 0 ? (rotationSpeed * dt) : -(rotationSpeed * dt);
				if (abs(deltaTheta) > PI / 2.f) {
					force = Vector2(std::cos(rotation), std::sin(rotation)) * maxForce;
				}
				else {
					force = Vector2(std::cos(rotation), std::sin(rotation)) * -maxForce;
				}
			}
		}
		// handle triggering, if sawn by player, then set target
		else {
			if (scene->GetCameraAABB().Intersects(GetAABB(curState))) {
				// find the closest player
				chaseTarget = scene->GetClosestPlayer(globalPosition);
			}
		}
		// handle despawn
		if (scene->IsInRoom(globalPosition)) {
			isActive = false;
			isPendingDestroy = true;
		}
		BaseCharacter::FixedUpdate(dt);
	}
};

