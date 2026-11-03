#include "SceneTally.h"
#include "ClickableText.h"
#include "SceneManager.h"
#include <string>

SceneTally::SceneTally(GamesEngineeringBase::Window& window, Camera& camera,
	SceneManager* manager, const RunStat s) : Scene(window, camera, manager){
	runStat = s;
	StartSceneTally();
}

void SceneTally::StartSceneTally() {
	for (int i = 0; i < uiObjects.GetSize(); ++i) {
		uiObjects[i]->isPendingDestroy = true;
	}

	AddUIObject(std::make_shared<ClickableText>(canvas, "GAME OVER", Vector2(200, 100), 6));
	AddUIObject(std::make_shared<ClickableText>(canvas, "BACK TO TITLE", Vector2(50, 625), 5,
		[this]() {
			sceneManager->ChangeScene({ SceneManagerState::MenuScene });
		}));

	int hour = (int)runStat.totalTime / 3600;
	int minute = (int)runStat.totalTime % 3600 / 60;
	int second = (int)runStat.totalTime % 60;

	AddUIObject(std::make_shared<ClickableText>(canvas,
		"TOTAL TIME: " + std::to_string(hour) + ":" + std::to_string(minute) + ":" + std::to_string(second),
		Vector2(50, 325), 5));
	AddUIObject(std::make_shared<ClickableText>(canvas, "ENEMY KILLED: " + std::to_string(runStat.totalEnemyKilled),
		Vector2(50, 400), 5));
}
