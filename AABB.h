#pragma once
#include "Vector3.h"

struct AABB {
    Vector3 min, max;
    AABB(Vector3 min, Vector3 max){
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
			return AABB(Vector3(0, 0, 0), Vector3(0, 0, 0));
		}

		Vector3 intersectionMin = Vector3(
			std::max(min.x, other.min.x),
			std::max(min.y, other.min.y),
			std::max(min.w, other.min.w)
		);
		Vector3 intersectionMax = Vector3(
			std::min(max.x, other.max.x),
			std::min(max.y, other.max.y),
			std::min(max.w, other.max.w)
		);
		return AABB(intersectionMin, intersectionMax);
	}
};

struct ScreenAABB { // Screen AABB is the AABB in screen space, pixel coordinates
	unsigned short minX, minY, maxX, maxY;
	ScreenAABB(unsigned short minX, unsigned short minY, unsigned short maxX, unsigned short maxY) {
		this->minX = minX;
		this->minY = minY;
		this->maxX = maxX;
		this->maxY = maxY;
	}
};