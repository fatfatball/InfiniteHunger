#include "Camera.h"
#include "BaseGameObject.h"
#include "Constants.h"
#include <cmath>
#include "Math.h"

void Camera::VisualUpdate(const float& dt) {
	Vector2 tmpPos = followTarget->renderState.globalPositionS;
	tmpPos.x = std::clamp(tmpPos.x, boundary.min.x, boundary.max.x);
	tmpPos.y = std::clamp(tmpPos.y, boundary.min.y, boundary.max.y);

	globalPosition = Math::Lerp(globalPosition, tmpPos, dt * 10);
}

void Camera::SetBoundary(const AABB& newBound) {
	// the input bound is where you can see
	// transfer that into the bound of camera pos
	boundary.min.x = newBound.min.x + FOV * ASPECT_RATIO / 2.f;
	boundary.max.x = newBound.max.x - FOV * ASPECT_RATIO / 2.f;
	boundary.min.y = newBound.min.y + FOV / 2.f;
	boundary.max.y = newBound.max.y - FOV / 2.f;
}