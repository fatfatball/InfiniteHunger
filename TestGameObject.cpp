#include "TestGameObject.h"
#include <algorithm>

void TestGameObject::OnStart() {
	prevState = { globalPosition, scale };
	curState = { globalPosition, scale };
}

void TestGameObject::FixedUpdate(const float& dt) {
	Vector2 input = Vector2();
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

	if (input.Length() > 0) {
		input = input.Normalized();
	}

	globalPosition = globalPosition + input * moveSpeed * dt;
}

void TestGameObject::VisualUpdate(const float& dt) {

}

ColorRGBA TestGameObject::GetColorAt(const float& u, const float& v) const {
	ColorRGBA color;
	float uu = std::clamp(u, 0.0f, 1.0f);
	float vv = std::clamp(v, 0.0f, 1.0f);

	color.r = uu * 255;
	color.g = vv * 255;
	color.b = 0;
	color.a = 255;

	return color;
}

void TestGameObject::UpdateStates() {
	prevState = curState;
	curState.globalPositionS = globalPosition;
	curState.scaleS = scale;
}

void TestGameObject::RenderSelf(const float& alpha, const Camera& camera) {
	AABB thisAABB = GetAABB(alpha);
	AABB cameraAABB = camera.GetAABB();
	if (!thisAABB.Intersects(cameraAABB)) {
		return;
	}

	AABB intersectionAABB = thisAABB.GetIntersectionAABB(cameraAABB);
	ScreenAABB screenAABB = camera.GetScreenAABB(intersectionAABB);
	for (int i = screenAABB.minX; i <= screenAABB.maxX; ++i) {
		for (int j = screenAABB.minY; j <= screenAABB.maxY; ++j) {
			Vector2 curGlobalPos = camera.GetGlobalPosition(i, j);
			float u = (curGlobalPos.x - thisAABB.min.x) / (thisAABB.max.x - thisAABB.min.x);
			float v = (curGlobalPos.y - thisAABB.min.y) / (thisAABB.max.y - thisAABB.min.y);
			ColorRGBA color = GetColorAt(u, v);
			camera.DrawOnCanvas(env, i, j, color);
		}
	}
}

AABB TestGameObject::GetAABB(const float& alpha) const {
	State renderState = GetRenderState(alpha);
	Vector2 min = Vector2(renderState.globalPositionS.x - renderState.scaleS.x / 2, renderState.globalPositionS.y - renderState.scaleS.y / 2);
	Vector2 max = Vector2(renderState.globalPositionS.x + renderState.scaleS.x / 2, renderState.globalPositionS.y + renderState.scaleS.y / 2);
	return AABB(min, max);
}

TestGameObject::State TestGameObject::GetRenderState(const float& alpha) const {
	State renderState;
	renderState.globalPositionS = prevState.globalPositionS * (1 - alpha) 
		+ curState.globalPositionS * alpha;
	renderState.scaleS = prevState.scaleS * (1 - alpha) + curState.scaleS * alpha;
	return renderState;
}
