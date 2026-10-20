#pragma once
#include "Vector2.h"
#include <algorithm>
// AABB and ScreenAABB

struct AABB { 
	// knwon issue: never check if max > min, or if AABB is so small that it can be ignored
    Vector2 min, max;

    AABB(Vector2 min, Vector2 max){
		this->min = min;
		this->max = max;
    }

	bool Intersects(const AABB& other) const {
		bool xOverlap = (min.x <= other.max.x) && (max.x >= other.min.x);
		bool yOverlap = (min.y <= other.max.y) && (max.y >= other.min.y);
		return xOverlap && yOverlap;
	}

	AABB GetIntersectionAABB(const AABB& other) const {
		if (!Intersects(other)) {
			return AABB(Vector2(0, 0), Vector2(0, 0));
		}

		Vector2 intersectionMin = Vector2(
			std::max(min.x, other.min.x),
			std::max(min.y, other.min.y)
		);
		Vector2 intersectionMax = Vector2(
			std::min(max.x, other.max.x),
			std::min(max.y, other.max.y)
		);
		return AABB(intersectionMin, intersectionMax);
	}
};

struct ScreenAABB { // Screen AABB is the AABB in screen space, pixel coordinates
	short minX, minY, maxX, maxY;
	ScreenAABB(short minX, short minY, short maxX, short maxY) {
		this->minX = minX;
		this->minY = minY;
		this->maxX = maxX;
		this->maxY = maxY;
	}
};