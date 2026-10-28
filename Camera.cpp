#include "Camera.h"
#include "BaseGameObject.h"
#include "Constants.h"
#include <cmath>

void Camera::VisualUpdate(const float& dt) {
	globalPosition = followTarget->renderState.globalPositionS;
	globalPosition.x = std::clamp(globalPosition.x, boundary.min.x, boundary.max.x);
	globalPosition.y  = std::clamp(globalPosition.y, boundary.min.y, boundary.max.y);
}

void Camera::SetBoundary(const AABB& newBound) {
	// the input bound is where you can see
	// transfer that into the bound of camera pos
	boundary.min.x = newBound.min.x + FOV * ASPECT_RATIO / 2.f;
	boundary.max.x = newBound.max.x - FOV * ASPECT_RATIO / 2.f;
	boundary.min.y = newBound.min.y + FOV / 2.f;
	boundary.max.y = newBound.max.y - FOV / 2.f;
}