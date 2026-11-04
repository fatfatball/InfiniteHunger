#pragma once
#include "Scene.h"
#include "GamesEngineeringBase.h"
#include "SmallStructs.h"
#include "SaveManager.h"

class SceneGameplay : public Scene {
public:
	RunStat runStat;
	SaveManager* saveManager;

	SceneGameplay(GamesEngineeringBase::Window& window, Camera& camera,
		SceneManager* manager, SaveManager* saveManager);

	void FixedUpdateObjects(const float& dt) override;
	void RecycleObjects() override;
	void PlayerAllDied();
};

