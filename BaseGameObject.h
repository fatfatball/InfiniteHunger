#pragma once
#include "Vector2.h"
#include "GamesEngineeringBase.h"
#include "AABB.h"
#include "Camera.h"
#include "Math.h"
#include "SmallStructs.h"

class Scene;

// base class for all game objects, including:
// trigger / renderable objects / spawner
class BaseGameObject {
protected:
	struct State {
		Vector2 globalPositionS;
		Vector2 scaleS;
		float rotationS = 0.f;
	}prevState, curState;

public:
	bool isActive = true;
	bool isPendingDestroy = false;
	GamesEngineeringBase::Window& env;
	Scene* scene;
	RenderOrder renderOrder;
	State renderState;

	Vector2 globalPosition = {};
	Vector2 scale = {}; // negative means flipped; directly represents the volume by global axis
	float rotation = 0.f; // in rad, 0 is facing right, pi/2 is facing down

	BaseGameObject(GamesEngineeringBase::Window& canvas) : env(canvas) {
		isActive = true;
		isPendingDestroy = false;
		scene = nullptr;
		renderOrder = RenderOrder::First;
		globalPosition = Vector2();
		scale = Vector2();
		rotation = 0.f;
		renderVisible = false;
	}
	virtual ~BaseGameObject() = default;

	virtual void OnStart() = 0;
	virtual void FixedUpdate(const float& dt) = 0;
	virtual void SolveConstraints() {} // for physical objects to override
	virtual void UpdateVelocity(const float& dt) {} // for physical objects to override
	virtual void VisualUpdate(const float& dt) = 0;
	virtual void OnDestroy() = 0;

	void SetRotation(const float& angle) {
		rotation = Math::WrapAngle(angle);
		curState.rotationS = rotation;
	}

	virtual ColorRGBA GetColorAt(const float& u, const float& v) const = 0; // sample the texture
	virtual unsigned char GetAlphaAt(const float& u, const float& v) const = 0;

	virtual void UpdateStates() { // to store the pos and scale into curState, and move the old curState to prevState
		prevState = curState;
		curState.globalPositionS = globalPosition;
		curState.scaleS = scale;
		curState.rotationS = rotation;
	}

	virtual bool DepthTest(float alpha, const Camera& camera, const short& curDepth) const {
		return DepthTest(alpha, camera, curDepth, camera.GetScreenAABB(camera.GetAABB()));
	}

	virtual bool DepthTest(float alpha, const Camera& camera, const short& curDepth, const ScreenAABB& tile) const {
		if (!renderVisible) {
			return false;
		}
		// check if this object intersects with camera's view
		if (!renderScreenAABB.Intersects(tile)) {
			return false;
		}

		// get the intersected AABB, traverse every pixel to draw
		ScreenAABB screenAABB = renderScreenAABB.GetIntersectionScreenAABB(tile);
		// pre calculate some values
		Vector2 beginPos = camera.GetGlobalPosition(screenAABB.minX, screenAABB.minY);
		beginPos = beginPos - renderState.globalPositionS;

		Vector2 localPos, rotatedPos, offset;
		float u, v;
		unsigned char colorAlpha;
		for (int j = screenAABB.minY; j <= screenAABB.maxY; ++j) {
			for (int i = screenAABB.minX; i <= screenAABB.maxX; ++i) {
				if (camera.depthGraph[j * WINDOW_WIDTH + i] < curDepth) {
					continue;
				}
				offset.x = (i - screenAABB.minX) * STEP_X;
				offset.y = (j - screenAABB.minY) * STEP_Y;
				localPos = beginPos + offset;
				// rotate -theta to get to the pos on texture
				rotatedPos.x = renderCosTheta * localPos.x + renderSinTheta * localPos.y;
				rotatedPos.y = renderCosTheta * localPos.y - renderSinTheta * localPos.x;

				if (std::abs(rotatedPos.x) > renderHalfScale.x || std::abs(rotatedPos.y) > renderHalfScale.y) {
					continue;
				}

				u = rotatedPos.x * renderInvScale.x + 0.5f;
				v = rotatedPos.y * renderInvScale.y + 0.5f;

				// if the alpha is 255, it blocks all other things behind
				colorAlpha = GetAlphaAt(u, v);
				if (colorAlpha == 255) {
					camera.depthGraph[j * WINDOW_WIDTH + i] = curDepth;
				}
			}
		}

		return true;
	}

	virtual void RenderSelf(float alpha, const Camera& camera, const short& curDepth) const {
		RenderSelf(alpha, camera, curDepth, camera.GetScreenAABB(camera.GetAABB()));
	}

	virtual void RenderSelf(float alpha, const Camera& camera, const short& curDepth, const ScreenAABB& tile) const {
		// render self in a specific tile
		if (!renderVisible) {
			return;
		}
		// check if this object intersects with current tile
		if (!renderScreenAABB.Intersects(tile)) {
			return;
		}
		
		// get the intersected AABB, traverse every pixel to draw
		ScreenAABB screenAABB = renderScreenAABB.GetIntersectionScreenAABB(tile);
		// pre calculate some values
		unsigned char* backBuffer = env.getBackBuffer();
		Vector2 beginPos = camera.GetGlobalPosition(screenAABB.minX, screenAABB.minY);
		beginPos = beginPos - renderState.globalPositionS;

		Vector2 localPos, rotatedPos, offset;
		float u, v;
		ColorRGBA color;
		for (int j = screenAABB.minY; j <= screenAABB.maxY; ++j) {
			for (int i = screenAABB.minX; i <= screenAABB.maxX; ++i) {
				if (camera.depthGraph[j * WINDOW_WIDTH + i] < curDepth) {
					continue;
				}
				offset.x = (i - screenAABB.minX) * STEP_X;
				offset.y = (j - screenAABB.minY) * STEP_Y;
				localPos = beginPos + offset;
				// rotate -theta to get to the pos on texture
				rotatedPos.x = renderCosTheta * localPos.x + renderSinTheta * localPos.y;
				rotatedPos.y = renderCosTheta * localPos.y - renderSinTheta * localPos.x;

				if (std::abs(rotatedPos.x) > renderHalfScale.x || std::abs(rotatedPos.y) > renderHalfScale.y) {
					continue;
				}

				u = rotatedPos.x * renderInvScale.x + 0.5f;
				v = rotatedPos.y * renderInvScale.y + 0.5f;
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

	virtual void UpdateRenderState(const float& alpha, const Camera& camera) {
		renderState.globalPositionS = Math::Lerp(prevState.globalPositionS, curState.globalPositionS, alpha);
		renderState.scaleS = Math::Lerp(prevState.scaleS, curState.scaleS, alpha);
		renderState.rotationS = Math::LerpAngle(prevState.rotationS, curState.rotationS, alpha);
		renderAABB = GetAABB(renderState);
		renderCosTheta = std::cos(renderState.rotationS);
		renderSinTheta = std::sin(renderState.rotationS);
		renderHalfScale = {
			std::abs(renderState.scaleS.x * 0.5f),
			std::abs(renderState.scaleS.y * 0.5f)
		};
		renderVisible = renderState.scaleS.x > 1 && renderState.scaleS.y > 1;
		if (renderVisible) renderInvScale = { 1 / renderState.scaleS.x , 1 / renderState.scaleS.y };
		renderScreenAABB = camera.GetScreenAABB(renderAABB);
	}

protected:
	// record some params for rendering to decrease calculation
	AABB renderAABB;
	float renderCosTheta = 0, renderSinTheta = 0;
	Vector2 renderHalfScale, renderInvScale;
	ScreenAABB renderScreenAABB;
	bool renderVisible;
};