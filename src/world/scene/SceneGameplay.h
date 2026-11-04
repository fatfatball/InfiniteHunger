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

	// for continue
	SceneGameplay(GamesEngineeringBase::Window& window, Camera& camera,
		SceneManager* manager, SaveManager* saveManager, const int& roomSeed,
		const RoomCoord& startRoom);

	void FixedUpdateObjects(const float& dt) override;
	void RecycleObjects() override;
	void PlayerAllDied();
};

