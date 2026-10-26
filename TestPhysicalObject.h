#pragma once
#include "BasePhysicalObject.h"
#include "TestPhysicalTail.h"
#include "List.h"
#include "SmallStructs.h"
#include "PolyLine2D.h"

// head for a spine monster
class TestPhysicalObject : public BasePhysicalObject {
public:
	float rotationSpeed;

	TestPhysicalObject(GamesEngineeringBase::Window& canvas);

	void OnStart() override;
	void FixedUpdate(const float& dt) override;
	void SolveConstraints() override;
	void VisualUpdate(const float& dt) override;
	void OnDestroy() override;
	ColorRGBA GetColorAt(const float& u, const float& v) const override;
	unsigned char GetAlphaAt(const float& u, const float& v) const override;

private:
	// store the body nodes for this monster. and the constraints between nodes.
	List<std::shared_ptr<BasePhysicalObject>> listTails;
	List<Constraint> constraints;
	PolyLine2D* polyLine2DPtr;

	// Solve constraints to establish physical effects.
	// Position Based Dynamics, https://doi.org/10.1016/j.jvcir.2007.01.005
	// here the param "stiffness" has no physical meaning. but it's simple, so I just use it.
	void UpdateConstraints();
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
};

