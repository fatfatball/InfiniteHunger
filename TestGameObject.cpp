#include "TestGameObject.h"
#include <algorithm>
#include "Math.h"

void TestGameObject::OnStart() {
	prevState = { globalPosition, scale, rotation };
	curState = { globalPosition, scale, rotation };
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

	globalPosition = globalPosition + inputDir * moveSpeed * dt;
	rotation = Math::WrapAngle(rotation + inputAngle * rotationSpeed * dt);
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
	curState.rotationS = rotation;
}

void TestGameObject::RenderSelf(const float& alpha, const Camera& camera) {
	State renderState = GetRenderState(alpha);
	AABB thisAABB = GetAABB(alpha, renderState);
	AABB cameraAABB = camera.GetAABB();
	if (!thisAABB.Intersects(cameraAABB)) {
		return;
	}

	AABB intersectionAABB = thisAABB.GetIntersectionAABB(cameraAABB);
	ScreenAABB screenAABB = camera.GetScreenAABB(intersectionAABB);
	const float cosTheta = std::cos(renderState.rotationS);
	const float sinTheta = std::sin(renderState.rotationS);
	Vector2 halfScale = renderState.scaleS * 0.5f;

	for (int i = screenAABB.minX; i <= screenAABB.maxX; ++i) {
		for (int j = screenAABB.minY; j <= screenAABB.maxY; ++j) {
			Vector2 curGlobalPos = camera.GetGlobalPosition(i, j);
			Vector2 localPos = curGlobalPos - renderState.globalPositionS;
			Vector2 rotatedPos = Vector2(
				cosTheta * localPos.x + sinTheta * localPos.y,
				cosTheta * localPos.y - sinTheta * localPos.x
			);

			if (std::abs(rotatedPos.x) > halfScale.x || std::abs(rotatedPos.y) > halfScale.y) {
				continue;
			}

			float u = rotatedPos.x / (halfScale.x * 2.0f) + 0.5f;
			float v = rotatedPos.y / (halfScale.y * 2.0f) + 0.5f;
			ColorRGBA color = GetColorAt(u, v);
			camera.DrawOnCanvas(env, i, j, color);
		}
	}
}

AABB TestGameObject::GetAABB(const float& alpha, const State& state) const {
	const float hx = state.scaleS.x / 2;
	const float hy = state.scaleS.y / 2;
	const float cosTheta = std::cos(state.rotationS);
	const float sinTheta = std::sin(state.rotationS);

	const float resX = std::abs(cosTheta * hx) + std::abs(sinTheta * hy);
	const float resY = std::abs(sinTheta * hx) + std::abs(cosTheta * hy);

	Vector2 min = Vector2(state.globalPositionS.x - resX, state.globalPositionS.y - resY);
	Vector2 max = Vector2(state.globalPositionS.x + resX, state.globalPositionS.y + resY);
	return AABB(min, max);
}

TestGameObject::State TestGameObject::GetRenderState(const float& alpha) const {
	State renderState;
	renderState.globalPositionS = Math::Lerp(prevState.globalPositionS, curState.globalPositionS, alpha);
	renderState.scaleS = Math::Lerp(prevState.scaleS, curState.scaleS, alpha);
	renderState.rotationS = Math::LerpAngle(prevState.rotationS, curState.rotationS, alpha);
	return renderState;
}
