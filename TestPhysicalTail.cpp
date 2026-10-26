#include "TestPhysicalTail.h"

TestPhysicalTail::TestPhysicalTail(GamesEngineeringBase::Window& canvas)
	: BasePhysicalObject(canvas) {
	scale = Vector2(40, 40);
	rotation = 0;
	maxVelocity = 600; // global position per second
	maxForce = 2000; // global position per second^2
	renderOrder = RenderOrder::Third;
	prevState = { globalPosition, scale, rotation };
	curState = { globalPosition, scale, rotation };
	canCollide = true;
	collisionRadius = 20;
}

TestPhysicalTail::TestPhysicalTail(GamesEngineeringBase::Window& canvas, const Vector2& pos)
	: TestPhysicalTail(canvas){
	//scale = Vector2(40, 40);
	//rotation = 0;
	//maxVelocity = 600; // global position per second
	//maxForce = 2000; // global position per second^2
	//renderOrder = RenderOrder::Third;
	globalPosition = pos;
	prevState = { globalPosition, scale, rotation };
	curState = { globalPosition, scale, rotation };
}

void TestPhysicalTail::OnStart() {
}

void TestPhysicalTail::FixedUpdate(const float& dt) {
	UpdatePhysicPosition(dt);
}

void TestPhysicalTail::VisualUpdate(const float& dt) {
}

void TestPhysicalTail::OnDestroy() {
}

ColorRGBA TestPhysicalTail::GetColorAt(const float& u, const float& v) const {
	ColorRGBA color;
	color.r = 255;
	color.g = 255;
	color.b = 255;
	color.a = GetAlphaAt(u, v);
	return color;
}

unsigned char TestPhysicalTail::GetAlphaAt(const float& u, const float& v) const
{
	// triangle facing right
	if ((v - 0.577f * u > -0.077f) && (v + 0.577f * u < 1.077f) && (u > 0.134)) {
		return 255;
	}
	else {
		return 0;
	}
}