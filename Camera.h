#pragma once
#include "Vector2.h"
#include "AABB.h"
#include "Constants.h"
#include "GamesEngineeringBase.h"
#include <algorithm>

class Camera
{
public:
	Vector2 globalPosition;

	Camera() {
		globalPosition = Vector2();
	}

	AABB GetAABB() const {
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
			std::clamp((short)minX, (short)0, (short)(WINDOW_WIDTH - 1)),
			std::clamp((short)minY, (short)0, (short)(WINDOW_HEIGHT - 1)), 
			std::clamp((short)maxX, (short)0, (short)(WINDOW_WIDTH - 1)),
			std::clamp((short)maxY, (short)0, (short)(WINDOW_HEIGHT - 1)));
		return screenAABB;
	}

	// Get the global position of a pixel in the camera's view
	Vector2 GetGlobalPosition(const int&x, const int&y) const {
		Vector2 res;
		res.x = ((float)x / WINDOW_WIDTH - 0.5f) * FOV * ASPECT_RATIO + globalPosition.x;
		res.y = ((float)y / WINDOW_HEIGHT - 0.5f) * FOV + globalPosition.y;
		return res;
	}

	void DrawOnCanvas(GamesEngineeringBase::Window& env, const int& x, const int& y, const ColorRGBA& color) const {
		if (x < 0 || x >= WINDOW_WIDTH || y < 0 || y >= WINDOW_HEIGHT) {
			return;
		}
		
		if (color.a == 255) {
			env.draw(x, y, color.r, color.g, color.b);
			return;
		}
		else if (color.a == 0) {
			return;
		}

		unsigned char* backBuffer = env.getBackBuffer();
		int index = ((y * WINDOW_WIDTH) + x) * 3;
		auto r = backBuffer[index];
		auto g = backBuffer[index + 1];
		auto b = backBuffer[index + 2];
		float alpha = color.a / 255.0f;
		backBuffer[index] = color.r * alpha + r * (1 - alpha);
		backBuffer[index + 1] = color.g * alpha + g * (1 - alpha);
		backBuffer[index + 2] = color.b * alpha + b * (1 - alpha);
	}
};

enum class RenderOrder {
	First,
	Second,
	Third,
};