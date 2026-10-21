#pragma once
#include "Vector2.h"
#include "AABB.h"
#include "Constants.h"
#include "GamesEngineeringBase.h"
#include <algorithm>
#include "SmallStructs.h"
#include <climits>

class Camera
{
public:
	Vector2 globalPosition;
	short* depthGraph; // smaller means more to the front, only for objects less than 0x7F7F

	Camera() {
		globalPosition = Vector2();
		depthGraph = new short[WINDOW_WIDTH * WINDOW_HEIGHT];
		ClearDepthGraph();
	}

	~Camera() {
		delete[] depthGraph;
	}

	inline AABB GetAABB() const {
		Vector2 min = Vector2(globalPosition.x - FOV * ASPECT_RATIO / 2, globalPosition.y - FOV / 2);
		Vector2 max = Vector2(globalPosition.x + FOV * ASPECT_RATIO / 2, globalPosition.y + FOV / 2);
		return AABB(min, max);
	}

	ScreenAABB GetScreenAABB(const AABB& aabb) const {
		float minX = (aabb.min.x - globalPosition.x) / (FOV * ASPECT_RATIO) * WINDOW_WIDTH + WINDOW_WIDTH / 2;
		float minY = (aabb.min.y - globalPosition.y) / FOV * WINDOW_HEIGHT + WINDOW_HEIGHT / 2;
		float maxX = (aabb.max.x - globalPosition.x) / (FOV * ASPECT_RATIO) * WINDOW_WIDTH + 1 + WINDOW_WIDTH / 2;
		float maxY = (aabb.max.y - globalPosition.y) / FOV * WINDOW_HEIGHT + 1 + WINDOW_HEIGHT / 2;\

		ScreenAABB screenAABB( 
			(short)std::clamp(minX, 0.f, (WINDOW_WIDTH - 1.f)),
			(short)std::clamp(minY, 0.f, (WINDOW_HEIGHT - 1.f)),
			(short)std::clamp(maxX, 0.f, (WINDOW_WIDTH - 1.f)),
			(short)std::clamp(maxY, 0.f, (WINDOW_HEIGHT - 1.f)));
		return screenAABB;
	}

	// Get the global position of a pixel in the camera's view
	inline Vector2 GetGlobalPosition(const int&x, const int&y) const {
		Vector2 res;
		res.x = ((float)x * INV_WINDOW_WIDTH - 0.5f) * FOV * ASPECT_RATIO + globalPosition.x;
		res.y = ((float)y * INV_WINDOW_HEIGHT - 0.5f) * FOV + globalPosition.y;
		return res;
	}

	void DrawOnCanvas(GamesEngineeringBase::Window& env, unsigned char*& backBuffer, const int& x, const int& y, const ColorRGBA& color) const {
		//if (x < 0 || x >= WINDOW_WIDTH || y < 0 || y >= WINDOW_HEIGHT) {
		//	return;
		//}
		if (color.a == 0) {
			return;
		}

		int index = ((y * WINDOW_WIDTH) + x) * 3;
		if (color.a == 255) {
			backBuffer[index] = color.r;
			backBuffer[index + 1] = color.g;
			backBuffer[index + 2] = color.b;
			return;
		}

		auto r = backBuffer[index];
		auto g = backBuffer[index + 1];
		auto b = backBuffer[index + 2];
		float alpha = color.a * INV255;
		float beta = 1 - alpha;
		backBuffer[index] = color.r * alpha + r * beta;
		backBuffer[index + 1] = color.g * alpha + g * beta;
		backBuffer[index + 2] = color.b * alpha + b * beta;
	}

	void ClearDepthGraph() const {
		memset(depthGraph, 0x7F, WINDOW_WIDTH * WINDOW_HEIGHT * sizeof(short));
	}
};