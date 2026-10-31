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
	float hitMovement; // how far player will be hit back, world axis

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
		hitMovement = 5.f;
	}

	BaseEnemy(GamesEngineeringBase::Window& canvas, const Vector2& pos) : BaseEnemy(canvas) {
		globalPosition = pos;
		curState.globalPositionS = globalPosition;
		prevState.globalPositionS = globalPosition;
	}

	void FixedUpdate(const float& dt) override {
		// handle movement after triggered
		if (chaseTarget != nullptr) {
			chaseTarget = scene->GetClosestPlayer(globalPosition);
		}
		if (chaseTarget != nullptr) {
			// re-capture the target
			Vector2 dir = chaseTarget->globalPosition - globalPosition;
			if (dir.Length() > 1e-4f) {
				float theta = std::atan2(dir.y, dir.x);
				float deltaTheta = Math::WrapAngle(theta - rotation);
				float turn = rotationSpeed * dt;
				if (deltaTheta > 0.f) {
					rotation = Math::WrapAngle(rotation + std::min(turn, std::abs(deltaTheta)));
				}
				else {
					rotation = Math::WrapAngle(rotation - std::min(turn, std::abs(deltaTheta)));
				}

				if (abs(deltaTheta) > PI * 2.f / 3.f) {
					// move backwards
					force = Vector2(std::cos(rotation), std::sin(rotation)) * -maxForce;
				}
				else if (abs(deltaTheta) > PI / 3.f) {
					// don't change velocity
					force = {};
				}
				else {
					// move forwards
					force = Vector2(std::cos(rotation), std::sin(rotation)) * maxForce;
				}
			}
		}
		// handle triggering, if sawn by player, then set target
		else {
			force.x = force.y = 0.f;
			if (scene->GetCameraAABB().Intersects(GetAABB(curState))) {
				// find the closest player
				chaseTarget = scene->GetClosestPlayer(globalPosition);
			}
		}
		// handle despawn
		if (!scene->GetCameraAABB().Intersects(GetAABB(curState)) && !scene->IsInRoom(globalPosition)) {
			isActive = false;
			isPendingDestroy = true;
			for (int i = 0; i < listTails.GetSize(); ++i) {
				listTails[i]->isPendingDestroy = true;
			}
			for (int i = 0; i < polyLine2Ds.GetSize(); ++i) {
				polyLine2Ds[i]->isPendingDestroy = true;
			}
		}
		BaseCharacter::FixedUpdate(dt);
	}

	void OnDestroy() override {
		for (int i = 0; i < listTails.GetSize(); ++i) {
			listTails[i]->TriggerWhenOwnerDie();
		}
		for (int i = 0; i < polyLine2Ds.GetSize(); ++i) {
			polyLine2Ds[i]->isPendingDestroy = true;
		}
	}

	void OnCollision(BaseGameObject* other, const Vector2& normal, const Vector2& position) override {
		if (contactDamage < 1e-4f) {
			return;
		}

		auto ptr1 = dynamic_cast<BasePlayer*>(other);
		if (ptr1) {
			if (ptr1->TakeDamage(contactDamage)) {
				ptr1->globalPosition = ptr1->globalPosition + normal * -hitMovement;
				globalPosition = globalPosition + normal * hitMovement;
			}
			return;
		}

		auto ptr2 = dynamic_cast<BaseBodyNode*>(other);
		if (ptr2 && ptr2->ownerType == CharacterType::Player) {
			if (ptr2->owner->TakeDamage(contactDamage)) {
				ptr2->owner->globalPosition = ptr2->owner->globalPosition + normal * -hitMovement;
				globalPosition = globalPosition + normal * hitMovement;
			}
			return;
		}
	}
};

