#pragma once
#include "Scene.h"
#include "SaveManager.h"

class SceneTutorial : public Scene {
public:
	SaveManager* s;

	SceneTutorial(GamesEngineeringBase::Window& window, Camera& camera,
		SceneManager* manager, SaveManager* saveManager);

	void PlayerAllDied();
};

