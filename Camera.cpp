#include "Camera.h"
#include "BaseGameObject.h"

void Camera::VisualUpdate(const float& dt) {
	globalPosition = followTarget->renderState.globalPositionS;
}
