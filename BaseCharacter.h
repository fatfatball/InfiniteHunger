#pragma once
#include "BasePhysicalObject.h"
#include "GamesEngineeringBase.h"
#include "List.h"
#include "PolyLine2D.h"
#include "SmallStructs.h"
#include <string>
#include <cmath>

class BaseBodyNode;

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

	List<std::shared_ptr<BaseBodyNode>> listTails;
	List<Constraint> distConstraints;
	List<Constraint> angleConstraints;
	List<RotationDefiner> rotationDefs;
	// these two are for updating the visual line
	List<std::shared_ptr<PolyLine2D>> polyLine2Ds;
	List<List<BasePhysicalObject*>> linedNodes;

	// Solve constraints to establish physical effects.
	// Position Based Dynamics, https://doi.org/10.1016/j.jvcir.2007.01.005
	// here the param "stiffness" has no physical meaning. but it's simple, so I just use it.
	// the dist between two points should stay the same.
	void UpdateDistanceConstraint(BasePhysicalObject* o1, BasePhysicalObject* o2,
		const float& targetDist, const float& stiffness);
	// the angle between two edges should be kept within maxBend.
	void UpdateAngleConstraint(BasePhysicalObject* o1, BasePhysicalObject* o2,
		BasePhysicalObject* o3,
		const float& restAngle, const float& maxBend, const float& stiffness);
	// the neck (fist edge after head) should be influenced by the rotation of head.
	void UpdateDirectionAngleConstraint(BasePhysicalObject* head, BasePhysicalObject* neckNode,
		const float& restAngle, const float& maxBend, const float& stiffness);

	virtual std::string GetSpritePath() = 0;

public:
	bool isDead; // sometimes the character is dead but should stay for a while

	BaseCharacter(GamesEngineeringBase::Window& canvas);

	void OnStart() override {
		sprite.load(GetSpritePath());
	}

	void FixedUpdate(const float& dt) override {
		InvulnerableTick(dt);
		UpdatePhysicPosition(dt);
	}
	// update constraints
	void SolveConstraints() override;

	void VisualUpdate(const float& dt) override;

	void OnDestroy() override;

	ColorRGBA GetColorAt(const float& u, const float& v) const override;

	unsigned char GetAlphaAt(const float& u, const float& v) const override;

	virtual bool TakeDamage(const float& damage);

	virtual bool Heal(const float& val);
	
	virtual void OnHurt() {};

	virtual void OnDie() {
		isPendingDestroy = true;
		isDead = true;
	}

	virtual void OnHeal() {};

	virtual void InvulnerableTick(const float& dt) {
		if (isInvulnerable) {
			invulnerableCounter -= dt;
			if (invulnerableCounter <= 0.f) {
				isInvulnerable = false;
			}
		}
	}
};

