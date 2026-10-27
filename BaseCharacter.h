#pragma once
#include "BasePhysicalObject.h"
#include "GamesEngineeringBase.h"
#include "List.h"
#include "PolyLine2D.h"
#include "SmallStructs.h"
#include <string>
#include <cmath>

// this is the abstract base class for players and enemies
class BaseCharacter : public BasePhysicalObject{
protected:
	GamesEngineeringBase::Image sprite;
	float rotationSpeed;
	float maxHealth;
	float curHealth;
	float invulnerableTime;
	float invulnerableCounter;
	bool isInvulnerable;

	List<std::shared_ptr<BasePhysicalObject>> listTails;
	List<Constraint> constraints;
	List<RotationDefiner> rotationDefs;
	// these two are for updating the visual line
	List<std::shared_ptr<PolyLine2D>> polyLine2Ds;
	List<List<BasePhysicalObject*>> linedNodes;

	// Solve constraints to establish physical effects.
	// Position Based Dynamics, https://doi.org/10.1016/j.jvcir.2007.01.005
	// here the param "stiffness" has no physical meaning. but it's simple, so I just use it.
	// the dist between two points should stay the same.
	void UpdateDistanceConstraint(BasePhysicalObject* o1, BasePhysicalObject* o2,
		const float& targetDist, const float& stiffness) {
			if (o1->isFixed && o2->isFixed) {
				return;
			}

			Vector2 vec = o2->globalPosition - o1->globalPosition;
			float dist = vec.Length();
			if (dist < 1e-4f) { // that's an error, just wait for the objects to split up
				return;
			}
			float diff = (dist - targetDist) * stiffness;

			if (o1->isFixed) {
				o2->globalPosition = o2->globalPosition - vec.Normalized() * diff;
			}
			else if (o2->isFixed) {
				o1->globalPosition = o1->globalPosition + vec.Normalized() * diff;
			}
			else {
				if (o1->mass < 1e-4f || o2->mass < 1e-4f) {
					o1->globalPosition = o1->globalPosition + vec.Normalized() * diff * 0.5f;
					o2->globalPosition = o2->globalPosition - vec.Normalized() * diff * 0.5f;
				}
				else {
					// to make sure momentum stays the same
					float w1 = 1.f / o1->mass;
					float w2 = 1.f / o2->mass;
					o1->globalPosition = o1->globalPosition + vec.Normalized() * diff * (w1 / (w1 + w2));
					o2->globalPosition = o2->globalPosition - vec.Normalized() * diff * (w2 / (w1 + w2));
				}
			}
	}
	// the angle between two edges should be kept within maxBend.
	void UpdateAngleConstraint(BasePhysicalObject* o1, BasePhysicalObject* o2,
		BasePhysicalObject* o3,
		const float& restAngle, const float& maxBend, const float& stiffness) {
		Vector2 v1 = o1->globalPosition - o2->globalPosition;
		Vector2 v2 = o3->globalPosition - o2->globalPosition;

		float l1 = v1.Length();
		float l2 = v2.Length();
		if (l1 < 1e-4f || l2 < 1e-4f) { // that's also an error
			return;
		}

		// calculate the angle from v1 to v2, [-pi, pi]
		float angle = std::atan2(v1.CrossProduct(v2), v1.DotProduct(v2));
		float delta = Math::AngleDifference(restAngle, angle); // [-pi, pi]

		if (delta >= -maxBend && delta <= maxBend) {
			return;
		}

		float targetDelta = std::clamp(delta, -maxBend, maxBend);
		float errAngle = delta - targetDelta; // [-pi, pi]

		// calculate grads so that points moves respectively
		Vector2 gradA = Vector2(v1.y, -v1.x) / (l1 * l1);
		Vector2 gradC = Vector2(-v2.y, v2.x) / (l2 * l2);
		Vector2 gradB = Vector2(
			-gradA.x - gradC.x,
			-gradA.y - gradC.y
		);

		// use mass as weight, please don't be 0
		float w1 = o1->isFixed ? 0.f : 1.f / o1->mass;
		float w2 = o2->isFixed ? 0.f : 1.f / o2->mass;
		float w3 = o3->isFixed ? 0.f : 1.f / o3->mass;

		float denom =
			w1 * (gradA.x * gradA.x + gradA.y * gradA.y) +
			w2 * (gradB.x * gradB.x + gradB.y * gradB.y) +
			w3 * (gradC.x * gradC.x + gradC.y * gradC.y);

		if (denom < 1e-4f) {
			return;
		}

		float lambda = stiffness * errAngle / denom;

		o1->globalPosition = o1->globalPosition - gradA * (lambda * w1);
		o2->globalPosition = o2->globalPosition - gradB * (lambda * w2);
		o3->globalPosition = o3->globalPosition - gradC * (lambda * w3);
	}
	// the neck (fist edge after head) should be influenced by the rotation of head.
	void UpdateDirectionAngleConstraint(BasePhysicalObject* head, BasePhysicalObject* neckNode,
		const float& restAngle, const float& maxBend, const float& stiffness) {
		Vector2 v1 = { std::cos(head->rotation), std::sin(head->rotation) };
		Vector2 v2 = neckNode->globalPosition - head->globalPosition;

		float l2 = v2.Length();
		if (l2 < 1e-4f) { // that's also an error
			return;
		}

		// calculate the angle from v1 to v2, [-pi, pi]
		float angle = std::atan2(v1.CrossProduct(v2), v1.DotProduct(v2));
		float delta = Math::AngleDifference(restAngle, angle); // [-pi, pi]

		if (delta >= -maxBend && delta <= maxBend) {
			return;
		}

		float targetDelta = std::clamp(delta, -maxBend, maxBend);
		float errAngle = delta - targetDelta; // [-pi, pi]

		// regard head as fixed, move neckNode
		float a = -errAngle * stiffness;
		Vector2 rotatedV2 = {
			std::cos(a) * v2.x - std::sin(a) * v2.y,
			std::cos(a) * v2.y + std::sin(a) * v2.x
		};
		neckNode->globalPosition = head->globalPosition + rotatedV2;
	}

	virtual std::string GetSpritePath() = 0;

public:
	BaseCharacter(GamesEngineeringBase::Window& canvas) : BasePhysicalObject(canvas) {
		maxHealth = 100.f;
		curHealth = maxHealth;
		scale = Vector2(50, 50);
		globalPosition = { 0, 0 };
		rotation = 0;
		maxVelocity = 600; // global position per second
		maxForce = 2000; // global position per second^2
		rotationSpeed = PI; // radians per second
		renderOrder = RenderOrder::Third;
		prevState = { globalPosition, scale, rotation };
		curState = { globalPosition, scale, rotation };
		isFixed = true;
		canCollide = true;
		collisionRadius = 20;
		invulnerableTime = 0.3;
		invulnerableCounter = invulnerableTime;
		isInvulnerable = false;
	}

	void OnStart() override {
		sprite.load(GetSpritePath());
	}

	void FixedUpdate(const float& dt) override {
		InvulnerableTick(dt);
		UpdatePhysicPosition(dt);
	}
	// update constraints
	void SolveConstraints() override {
		// Gauss-Seidel method, update the constraints one by one
		// here is from head to tail
		// Jacobi method will first calculate all constraints and the gradient, then move them all.
		// but it's just too complicated to imply
		int size = constraints.GetSize();
		// only iterate one time here. the more iteration, the stiffer it would be
		for (int i = 0; i < size; ++i) {
			switch (constraints[i].type) {
			case ConstraintType::Distance:
				UpdateDistanceConstraint(constraints[i].o1, constraints[i].o2,
					constraints[i].constraint1, constraints[i].stiffness);
				break;
			case ConstraintType::Angle:
				UpdateAngleConstraint(constraints[i].o1, constraints[i].o2, constraints[i].o3,
					constraints[i].constraint1, constraints[i].constraint2, constraints[i].stiffness);
				break;
			case ConstraintType::DirectionAngle:
				UpdateDirectionAngleConstraint(constraints[i].o1, constraints[i].o2,
					constraints[i].constraint1, constraints[i].constraint2, constraints[i].stiffness);
				break;
			default:
				break;
			}
		}
	}

	void VisualUpdate(const float& dt) override {
		// update the polyline2D
		for (int i = 0; i < linedNodes.GetSize(); ++i) {
			for (int j = 0; j < linedNodes[i].GetSize(); ++j) {
				polyLine2Ds[i]->points[j] = linedNodes[i][j]->renderState.globalPositionS;
			}
		}

		// update the rotation of each body node
		Vector2 dir;
		float angle;
		for (int i = 0; i < rotationDefs.GetSize(); ++i) {
			switch (rotationDefs[i].type) {
			case RotationDefType::PrevAndNext:
				dir = rotationDefs[i].o1->globalPosition - rotationDefs[i].o3->globalPosition;
				if (dir.Length() < 1e-4f) {
					break;
				}
				angle = std::atan2(dir.y, dir.x);
				rotationDefs[i].o2->SetRotation(angle);
				break;
			case RotationDefType::Prev:
				dir = rotationDefs[i].o1->globalPosition - rotationDefs[i].o2->globalPosition;
				if (dir.Length() < 1e-4f) {
					break;
				}
				angle = std::atan2(dir.y, dir.x);
				rotationDefs[i].o2->SetRotation(angle);
				break;
			default:
				break;
			}
		}
	}

	void OnDestroy() override {
		for (int i = 0; i < listTails.GetSize(); ++i) {
			listTails[i]->isPendingDestroy = true;
		}
		for (int i = 0; i < polyLine2Ds.GetSize(); ++i) {
			polyLine2Ds[i]->isPendingDestroy = true;
		}
	}

	ColorRGBA GetColorAt(const float& u, const float& v) const override {
		int x = std::clamp(u, 0.f, 1.f) * sprite.width;
		int y = std::clamp(v, 0.f, 1.f) * sprite.height;
		auto ptr = sprite.atUnchecked(x, y);
		ColorRGBA color;
		color.r = ptr[0];
		color.g = ptr[1];
		color.b = ptr[2];
		color.a = sprite.alphaAtUnchecked(x, y);
		return color;
	}

	unsigned char GetAlphaAt(const float& u, const float& v) const override {
		int x = std::clamp(u, 0.f, 1.f) * sprite.width;
		int y = std::clamp(v, 0.f, 1.f) * sprite.height;
		auto ptr = sprite.atUnchecked(x, y);
		return sprite.alphaAtUnchecked(x, y);
	}

	virtual void TakeDamage(const float& damage) {
		if (isPendingDestroy || isInvulnerable || curHealth <= 0.f) {
			return;
		}

		curHealth -= damage;
		invulnerableCounter = invulnerableTime;
		isInvulnerable = true;

		if (curHealth <= 0.f) {
			curHealth = 0.f;
			OnDie();
		}

		return;
	}

	virtual void Heal(const float& val) {
		if (isPendingDestroy || curHealth <= 0.f) {
			return;
		}
		curHealth += val;
		if (curHealth > maxHealth) {
			curHealth = maxHealth;
		}
	}
	
	virtual void OnDie() {
		isPendingDestroy = true;
	}

	virtual void InvulnerableTick(const float& dt) {
		if (isInvulnerable) {
			invulnerableCounter -= dt;
			if (invulnerableCounter <= 0.f) {
				isInvulnerable = false;
			}
		}
	}
};

