#include "TestGameObject.h"
#include <algorithm>
#include "Math.h"
#include <iostream>

TestGameObject::TestGameObject(GamesEngineeringBase::Window& canvas) 
: BaseGameObject(canvas) {
	scale = Vector2(500, 500); // a square
	rotation = 0;
	moveSpeed = 600; // global position per second
	rotationSpeed = PI; // radians per second
	renderOrder = RenderOrder::Third;
	prevState = { globalPosition, scale, rotation };
	curState = { globalPosition, scale, rotation };
}

void TestGameObject::OnStart() {
	
}

void TestGameObject::FixedUpdate(const float& dt) {
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

	globalPosition = globalPosition + inputDir * moveSpeed * dt;
	rotation = Math::WrapAngle(rotation + inputAngle * rotationSpeed * dt);
}

void TestGameObject::VisualUpdate(const float& dt) {

}

void TestGameObject::OnDestroy() {
	std::cout << "I'm destroyed!" << std::endl;
}

ColorRGBA TestGameObject::GetColorAt(const float& u, const float& v) const {
	ColorRGBA color;
	float uu = std::clamp(u, 0.0f, 1.0f);
	float vv = std::clamp(v, 0.0f, 1.0f);

	color.r = (unsigned char)(uu * 255);
	color.g = (unsigned char)(vv * 255);
	color.b = (unsigned char)0;
	color.a = (unsigned char)255;

	return color;
}