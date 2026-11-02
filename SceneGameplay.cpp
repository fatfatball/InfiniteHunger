#include "SceneGameplay.h"

SceneGameplay::SceneGameplay(GamesEngineeringBase::Window& window, Camera& camera) : Scene(window, camera) {
	// initialize the room manager
	roomManager = std::make_shared<RoomManager>(canvas);
	roomManager->scene = this;
	AddObject(roomManager);
}
