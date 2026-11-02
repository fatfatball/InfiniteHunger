#include "SceneGameplay.h"

SceneGameplay::SceneGameplay(GamesEngineeringBase::Window& window, Camera& camera, SceneManager* manager) 
	: Scene(window, camera, manager) {
	// initialize the room manager
	roomManager = std::make_shared<RoomManager>(canvas);
	roomManager->scene = this;
	AddObject(roomManager);
}
