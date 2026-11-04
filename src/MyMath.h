#pragma once
#include <cmath>
#include "Constants.h"
#include "Vector2.h"
#include "AABB.h"

class Vector2;
struct ScreenAABB;

class Math
{
public:
	template<typename T>
	static T Lerp(const T& a, const T& b, const float& alpha) {
		return a * (1 - alpha) + b * alpha;
	}
	
	static float WrapAngle(const float& angle) {
		return std::remainder<float>(angle, TWO_PI);
	}

	static float AngleDifference(const float& from, const float& to) {
		return WrapAngle(to - from);
	}

	static float LerpAngle(const float& a, const float& b, const float& alpha) {
		float diff = AngleDifference(a, b);
		return WrapAngle(a + diff * alpha);
	}

    // Liang-Barsky clipping algorithm
    // changes the clipped params
	static bool ClipLineToTile(Vector2& a, Vector2& b, const ScreenAABB& tile) {
        float x0 = a.x;
        float y0 = a.y;
        float x1 = b.x;
        float y1 = b.y;

        float dx = x1 - x0;
        float dy = y1 - y0;

        float t0 = 0.f;
        float t1 = 1.f;

        auto clip = [&](float p, float q) {
            if (std::abs(p) < 1e-4f) return q >= 0.f;

            float r = q / p;
            if (p < 0.f) {
                if (r > t1) {
                    return false;
                }
                if (r > t0) {
                    t0 = r;
                }
            }
            else {
                if (r < t0) {
                    return false;
                }
                if (r < t1) {
                    t1 = r;
                }
            }
            return true;
            };

        if (!clip(-dx, x0 - tile.minX)) {
            return false;
        }
        if (!clip(dx, tile.maxX - x0)) {
            return false;
        }
        if (!clip(-dy, y0 - tile.minY)) {
            return false;
        }
        if (!clip(dy, tile.maxY - y0)) {
            return false;
        }

        float nx0 = x0 + t0 * dx;
        float ny0 = y0 + t0 * dy;
        float nx1 = x0 + t1 * dx;
        float ny1 = y0 + t1 * dy;

        a = Vector2(nx0, ny0);
        b = Vector2(nx1, ny1);
        return true;
    }
};

