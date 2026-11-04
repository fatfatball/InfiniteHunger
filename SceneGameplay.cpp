#include "SceneGameplay.h"
#include "BasePlayer.h"
#include "BaseEnemy.h"
#include "ObjectTimer.h"
#include "SceneManager.h"
#include "SmallStructs.h"
#include "SaveManager.h"

SceneGameplay::SceneGameplay(GamesEngineeringBase::Window& window, Camera& camera,
	SceneManager* manager, SaveManager* saveManager)
	: Scene(window, camera, manager) {
	// initialize the room manager
	roomManager = std::make_shared<RoomManager>(canvas, saveManager);
	roomManager->scene = this;
	AddObject(roomManager);
	runStat = { manager->GetRunStat().playerCount, 0, 0.0 };
	this->saveManager = saveManager;
}

void SceneGameplay::FixedUpdateObjects(const float& dt) {
	Scene::FixedUpdateObjects(dt);
	runStat.totalTime += dt;
}

void SceneGameplay::RecycleObjects() {
	int size = objects.GetSize();
	for (int i = size - 1; i >= 0; --i) {
		if (!objects[i]->isPendingDestroy) {
			continue;
		}
		// clear other places
		auto ptr = objects[i].get();
		if (camera.followTarget == ptr) {
			camera.followTarget = nullptr;
		}
		auto playerPtr = dynamic_cast<BasePlayer*>(ptr);
		if (playerPtr) {
			// Handle player-specific cleanup
			for (int j = 0; j < players.GetSize(); ++j) {
				if (players[j].get() == playerPtr) {
					players.Erase(j);
					break;
				}
			}
		}
		else if (auto enemyPtr = dynamic_cast<BaseEnemy*>(ptr)) {
			// Handle enemy-specific cleanup
			++runStat.totalEnemyKilled;
		}

		objects[i]->OnDestroy();
		objects.Erase(i);
	}

	// recycle UI objects
	size = uiObjects.GetSize();
	for (int i = size - 1; i >= 0; --i) {
		if (!uiObjects[i] || !uiObjects[i]->isPendingDestroy) {
			continue;
		}
		uiObjects[i]->OnDestroy();
		uiObjects.Erase(i);
	}
}

void SceneGameplay::PlayerAllDied() {
	// wait 3 seconds, then go to tally scene
	auto ptr = std::make_shared<ObjectTimer>(canvas);
	ptr->SetTimer(3.f, [this]() {
		sceneManager->ChangeScene({ SceneManagerState::TallyScene, PlayerType::Archer, PlayerType::Archer, runStat});
		});
	ptr->StartTimer();
	AddObject(ptr);
}
