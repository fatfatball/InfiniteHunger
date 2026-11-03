#pragma once
#include "Scene.h"
#include "SmallStructs.h"

class SceneTally : public Scene {
public:
	SceneTally(GamesEngineeringBase::Window& window, Camera& camera, SceneManager* manager, const RunStat s);

	void StartSceneTally();

private:
	RunStat runStat;
};

