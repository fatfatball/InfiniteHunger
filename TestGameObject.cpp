#include "TestGameObject.h"

void TestGameObject::OnStart() {
	prevState = { globalPosition, scale };
	curState = { globalPosition, scale };
}

void TestGameObject::FixedUpdate(float dt) {
	Vector3 input = Vector3();
	if (env.keyPressed(0x57)) { // W
		input.y += -1;
	}
	if (env.keyPressed(0x41)) { // A
		input.x += -1;
	}
	if (env.keyPressed(0x53)) { // S
		input.y += 1;
	}
	if (env.keyPressed(0x44)) { // D
		input.x += 1;
	}
	input = input.Normalized();

	globalPosition = globalPosition + input * moveSpeed * dt;
}

void TestGameObject::VisualUpdate(float dt) {

}

ColorRGB TestGameObject::GetColorAt(float u, float v) {
	ColorRGB color;
	color.r = u * 255;
	color.g = v * 255;

	return color;
}

void TestGameObject::UpdateStates() {
	prevState = curState;
	curState.globalPositionS = globalPosition;
	curState.scaleS = scale;
}

TestGameObject::State TestGameObject::GetRenderState(float alpha) {
	State renderState;
	renderState.globalPositionS = prevState.globalPositionS * (1 - alpha) 
		+ curState.globalPositionS * alpha;
	renderState.scaleS = prevState.scaleS * (1 - alpha) + curState.scaleS * alpha;
	return renderState;
}
