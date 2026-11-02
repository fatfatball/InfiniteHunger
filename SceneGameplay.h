#pragma once
#include "Scene.h"
#include "GamesEngineeringBase.h"

class SceneGameplay : public Scene {
public:
	SceneGameplay(GamesEngineeringBase::Window& window, Camera& camera);
};

