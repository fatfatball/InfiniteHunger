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

	void UpdateConstraints();
	void UpdateDistanceConstraint(BasePhysicalObject* o1, BasePhysicalObject* o2, const float& targetDist);
};

