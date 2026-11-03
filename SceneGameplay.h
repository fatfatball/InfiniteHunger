#pragma once
#include "Scene.h"
#include "GamesEngineeringBase.h"
#include "SmallStructs.h"

class SceneGameplay : public Scene {
private:
	RunStat runStat;

public:
	SceneGameplay(GamesEngineeringBase::Window& window, Camera& camera, SceneManager* manager);

	void FixedUpdateObjects(const float& dt) override;
	void RecycleObjects() override;
	void PlayerAllDied();
};

