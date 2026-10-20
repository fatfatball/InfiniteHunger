#pragma once
#include "Vector2.h"
#include "GamesEngineeringBase.h"
#include "AABB.h"
#include "Camera.h"
#include "Math.h"
#include "SmallStructs.h"

// base class for all game objects, including:
// trigger / renderable objects / spawner
class Scene;

class BaseGameObject
{
protected:
	struct State {
		Vector2 globalPositionS;
		Vector2 scaleS;
		float rotationS = 0.f;
	};

public:
	bool isActive = true;
	bool isPendingDestroy = false;
	GamesEngineeringBase::Window& env;
	Scene* scene;
	RenderOrder renderOrder;

	Vector2 globalPosition = {};
	Vector2 scale = {}; // negative means flipped; directly represents the volume by global axis
	float rotation = 0.f; // in rad, 0 is facing right, pi/2 is facing up

	BaseGameObject(GamesEngineeringBase::Window& canvas) : env(canvas) {
		isActive = true;
		isPendingDestroy = false;
		scene = nullptr;
		renderOrder = RenderOrder::First;
		globalPosition = Vector2();
		scale = Vector2();
		rotation = 0.f;
	}
	virtual ~BaseGameObject() = default;

	virtual void OnStart() = 0;
	virtual void FixedUpdate(const float& dt) = 0;
	virtual void VisualUpdate(const float& dt) = 0;
	virtual void OnDestroy() = 0;

	virtual ColorRGBA GetColorAt(const float& u, const float& v) const = 0; // sample the texture

	void UpdateStates() { // to store the pos and scale into curState, and move the old curState to prevState
		prevState = curState;
		curState.globalPositionS = globalPosition;
		curState.scaleS = scale;
		curState.rotationS = rotation;
	}

	virtual void RenderSelf(float alpha, const Camera& camera) {
		State renderState = GetRenderState(alpha);
		if (renderState.scaleS.Length() < 1) {
			return;
		}
		// check if this object intersects with camera's view
		AABB thisAABB = GetAABB(renderState);
		AABB cameraAABB = camera.GetAABB();
		if (!thisAABB.Intersects(cameraAABB)) {
			return;
		}

		// get the intersected AABB, traverse every pixel to draw
		AABB intersectionAABB = thisAABB.GetIntersectionAABB(cameraAABB);
		ScreenAABB screenAABB = camera.GetScreenAABB(intersectionAABB);
		// pre calculate some values
		const float cosTheta = std::cos(renderState.rotationS);
		const float sinTheta = std::sin(renderState.rotationS);
		Vector2 halfScale = {
			std::abs(renderState.scaleS.x * 0.5f),
			std::abs(renderState.scaleS.y * 0.5f)
		};
		Vector2 invScale = { 1 / renderState.scaleS.x , 1 / renderState.scaleS.y };
		unsigned char* backBuffer = env.getBackBuffer();
		Vector2 beginPos = camera.GetGlobalPosition(screenAABB.minX, screenAABB.minY);
		beginPos = beginPos + camera.globalPosition - renderState.globalPositionS;

		Vector2 localPos, rotatedPos, offset;
		float u, v;
		ColorRGBA color;
		for (int j = screenAABB.minY; j <= screenAABB.maxY; ++j) {
			for (int i = screenAABB.minX; i <= screenAABB.maxX; ++i) {
				offset.x = (i - screenAABB.minX) * STEP_X;
				offset.y = (j - screenAABB.minY) * STEP_Y;
				localPos = beginPos + offset;
				// rotate -theta to get to the pos on texture
				rotatedPos.x = cosTheta * localPos.x + sinTheta * localPos.y;
				rotatedPos.y = cosTheta * localPos.y - sinTheta * localPos.x;

				if (std::abs(rotatedPos.x) > halfScale.x || std::abs(rotatedPos.y) > halfScale.y) {
					continue;
				}

				u = rotatedPos.x * invScale.x + 0.5f;
				v = rotatedPos.y * invScale.y + 0.5f;
				color = GetColorAt(u, v);
				camera.DrawOnCanvas(env, backBuffer, i, j, color);
			}
		}
	}

	AABB GetAABB(const State& state) const {
		const float hx = std::abs(state.scaleS.x / 2);
		const float hy = std::abs(state.scaleS.y / 2);
		const float cosTheta = std::cos(state.rotationS);
		const float sinTheta = std::sin(state.rotationS);

		const float resX = std::abs(cosTheta * hx) + std::abs(sinTheta * hy);
		const float resY = std::abs(sinTheta * hx) + std::abs(cosTheta * hy);

		Vector2 min = Vector2(state.globalPositionS.x - resX, state.globalPositionS.y - resY);
		Vector2 max = Vector2(state.globalPositionS.x + resX, state.globalPositionS.y + resY);
		return AABB(min, max);
	}

	virtual AABB GetAABB() {
		return GetAABB(curState);
	}

	State GetCurState() const {
		return curState; 
	}

protected:
	State prevState, curState;

	State GetRenderState(const float& alpha) const {
		State renderState;
		renderState.globalPositionS = Math::Lerp(prevState.globalPositionS, curState.globalPositionS, alpha);
		renderState.scaleS = Math::Lerp(prevState.scaleS, curState.scaleS, alpha);
		renderState.rotationS = Math::LerpAngle(prevState.rotationS, curState.rotationS, alpha);
		return renderState;
	}
};