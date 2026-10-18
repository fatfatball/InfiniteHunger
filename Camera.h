#pragma once
#include "Vector3.h"
#include "AABB.h"
#include "Constants.h"
#include "GamesEngineeringBase.h"

class Camera
{
public:
	Vector3 globalPosition;

	Camera() {
		globalPosition = Vector3();
	}

	AABB GetAABB() const {
		Vector3 min = Vector3(globalPosition.x - FOV * ASPECT_RATIO / 2, globalPosition.y - FOV / 2, 1);
		Vector3 max = Vector3(globalPosition.x + FOV * ASPECT_RATIO / 2, globalPosition.y + FOV / 2, 1);
		return AABB(min, max);
	}

	ScreenAABB GetScreenAABB(const AABB& aabb) const {
		ScreenAABB screenAABB(0, 0, 0, 0);
		screenAABB.minX = ((aabb.min.x - globalPosition.x) / (FOV * ASPECT_RATIO) * WINDOW_WIDTH) + WINDOW_WIDTH / 2;
		screenAABB.minY = ((aabb.min.y - globalPosition.y) / FOV * WINDOW_HEIGHT) + WINDOW_HEIGHT / 2;
		screenAABB.maxX = ((aabb.max.x - globalPosition.x) / (FOV * ASPECT_RATIO) * WINDOW_WIDTH) + 1 + WINDOW_WIDTH / 2;
		screenAABB.maxY = ((aabb.max.y - globalPosition.y) / FOV * WINDOW_HEIGHT) + 1 + WINDOW_HEIGHT / 2;
		if (screenAABB.minX < 0) {
			screenAABB.minX = 0;
		}
		if (screenAABB.minY < 0) {
			screenAABB.minY = 0;
		}
		if (screenAABB.maxX > WINDOW_WIDTH - 1) {
			screenAABB.maxX = WINDOW_WIDTH - 1;
		}
		if (screenAABB.maxY > WINDOW_HEIGHT - 1) {
			screenAABB.maxY = WINDOW_HEIGHT - 1;
		}
		return screenAABB;
	}

	// Get the global position of a pixel in the camera's view
	Vector3 GetGlobalPosition(const int&x, const int&y) const {
		Vector3 res;
		res.x = ((float)x / WINDOW_WIDTH - 0.5f) * FOV * ASPECT_RATIO + globalPosition.x;
		res.y = ((float)y / WINDOW_HEIGHT - 0.5f) * FOV + globalPosition.y;
		res.w = 1;
		return res;
	}

	void DrawOnCanvas(GamesEngineeringBase::Window& env, const int& x, const int& y, const ColorRGBA& color) const {
		unsigned char* backBuffer = env.getBackBuffer();
		int index = ((y * WINDOW_WIDTH) + x) * 3;
		auto r = backBuffer[index];
		auto g = backBuffer[index + 1];
		auto b = backBuffer[index + 2];
		backBuffer[index] = color.r * color.a + r * (1 - color.a);
		backBuffer[index + 1] = color.g * color.a + g * (1 - color.a);
		backBuffer[index + 2] = color.b * color.a + b * (1 - color.a);
	}
};

