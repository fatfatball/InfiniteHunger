#include "TestPhysicalObject.h"
#include <iostream>

TestPhysicalObject::TestPhysicalObject(GamesEngineeringBase::Window& canvas) : BasePhysicalObject(canvas) {
	scale = Vector2(100, 100);
	rotation = 0;
	moveSpeed = 500; // global position per second
	moveForce = 2000; // global position per second^2
	rotationSpeed = PI; // radians per second
	renderOrder = RenderOrder::Third;
	prevState = { globalPosition, scale, rotation };
	curState = { globalPosition, scale, rotation };
}

void TestPhysicalObject::OnStart() {
	
}

void TestPhysicalObject::FixedUpdate(const float& dt) {
	Vector2 inputDir = Vector2();
	if (env.keyPressed(0x57)) { // W
		inputDir.y += -1;
	}
	if (env.keyPressed(0x41)) { // A
		inputDir.x += -1;
	}
	if (env.keyPressed(0x53)) { // S
		inputDir.y += 1;
	}
	if (env.keyPressed(0x44)) { // D
		inputDir.x += 1;
	}

	if (inputDir.Length() > 0) {
		inputDir = inputDir.Normalized();
	}

	float inputAngle = 0;
	if (env.keyPressed(0x51)) { // Q
		inputAngle += 1;
	}
	if (env.keyPressed(0x45)) { // E
		inputAngle -= 1;
	}

	if (env.keyPressed(0x20)) { // space
		isPendingDestroy = true;
	}

	// the force calculation algorithm is still naive
	// TODO: make it more natural, use Steering Behaviors
	force = inputDir * std::clamp(((inputDir * moveSpeed - curPhysicState.velocityS).Length() / moveSpeed), 0.f, 1.f) * moveForce;
	rotation = Math::WrapAngle(rotation + inputAngle * rotationSpeed * dt);
	UpdatePhysicStates(dt);
}

void TestPhysicalObject::VisualUpdate(const float& dt) {

}

void TestPhysicalObject::OnDestroy() {
	std::cout << "I'm destroyed!" << std::endl;
}

ColorRGBA TestPhysicalObject::GetColorAt(const float& u, const float& v) const {
	ColorRGBA color;
	color.r = 255;
	color.g = 255;
	color.b = 255;
	color.a = GetAlphaAt(u, v);
	return color;
}

unsigned char TestPhysicalObject::GetAlphaAt(const float& u, const float& v) const
{
	if ((v - 1.732f * u > -0.732f) && (v + 1.732f * u > 1)) {
		return 255;
	}
	else {
		return 0;
	}
}
