#include "SceneManager.h"
#include "SceneGameplay.h"
#include <memory>
#include "TextRenderer.h"
#include "SceneMenu.h"
#include "SceneTally.h"
#include "SmallStructs.h"

void SceneManager::SolveSceneChange() {
	if (nextScene) {
		currentScene.reset();
		currentScene = std::move(nextScene);
		nextScene.reset();
	}
}

SceneManager::SceneManager(bool& running) : running(running) {
	canvas.create(WINDOW_WIDTH, WINDOW_HEIGHT, "Infinite Hunger");
	// initialize the current scene into menu scene
	// test for single scene
	camera.ResetCamera();
	ChangeScene({ SceneManagerState::MenuScene, PlayerType::Swordsman, PlayerType::Archer,{} });
	SolveSceneChange();
	curState = SceneManagerState::None;
	isPendingQuit = false;
}

void SceneManager::ChangeScene(const SceneChangeContext& ctx) {
	runStat = ctx.stat;
	camera.ResetCamera();
	curState = ctx.nextScene;
	switch (ctx.nextScene) {
	case SceneManagerState::None:
		break;
	case SceneManagerState::SingleScene:
		saveManager.Clear();
		nextScene = std::make_unique<SceneGameplay>(canvas, camera, this, &saveManager);
		nextScene->AddPlayer(ctx.player1);
		break;
	case SceneManagerState::MultiScene:
		saveManager.Clear();
		nextScene = std::make_unique<SceneGameplay>(canvas, camera, this, &saveManager);
		nextScene->AddPlayer(ctx.player1);
		nextScene->AddPlayer(ctx.player2);
		break;
	case SceneManagerState::LoadScene:
		ContinueGame(SAVE_FILE);
		break;
	case SceneManagerState::MenuScene:
		nextScene = std::make_unique<SceneMenu>(canvas, camera, this);
		break;
	case SceneManagerState::TallyScene:
		nextScene = std::make_unique<SceneTally>(canvas, camera, this, runStat);
		break;
	default:
		break;
	}
}

void SceneManager::FixedUpdate(const float& dt) {
	if (!currentScene) {
		return;
	}
	currentScene->FixedUpdateObjects(dt);
	currentScene->RecycleObjects();

	if (canvas.keyPressed(0x1b)) { // esc to close
		QuitGame();
	}
}

void SceneManager::VisualUpdate(const float& dt) {
	if (!currentScene) {
		return;
	}
	currentScene->VisualUpdateObjects(dt);
	camera.VisualUpdate(dt);

	if (isPendingQuit) {
		// recycle everything
		currentScene->RecycleAllObjects(); // this will leave players exist
		// if it is single player / multi player, then save to file.
		if (curState == SceneManagerState::SingleScene || curState == SceneManagerState::MultiScene) {
			auto curScene = dynamic_cast<SceneGameplay*>(currentScene.get());
			SaveContext saveCtx;
			saveCtx.roomSeed = curScene->GetRoomManager()->GetRandSeed();
			saveCtx.startRoom = curScene->GetRoomManager()->prevRoom;
			saveCtx.runStat = curScene->runStat;
			saveCtx.playerCount = curScene->GetPlayerCount();
			for (int i = 0; i < saveCtx.playerCount; ++i) {
				saveCtx.playerSave[i] = curScene->GetPlayerSaveDataAt(i);
			}

			// only if there is alive player, can it be saved
			if (saveCtx.playerSave[0].isAlive || saveCtx.playerSave[1].isAlive) {
				saveManager.SaveFile(SAVE_FILE, saveCtx);
			}
		}
	}
}

void SceneManager::Render(const float& alpha, const int& fpsCount) {
	if (!currentScene) {
		return;
	}
	canvas.clear();
	currentScene->RenderObjects(alpha);
	TextRenderer::DrawText(canvas, "FPS: " + std::to_string(fpsCount), 5, 5, 2, { 127, 127, 127, 255 });
	canvas.present();
	SolveSceneChange();
}

void SceneManager::QuitGame() {
	running = false;
	isPendingQuit = true;
}

bool SceneManager::ContinueGame(std::string path) {
	SaveContext ctx;

	if (!saveManager.LoadFile(path, ctx)) {
		return false;
	}

	camera.ResetCamera();
	runStat = ctx.runStat;
	auto gameplayScene = std::make_unique<SceneGameplay>(canvas, camera, this, &saveManager,
		ctx.roomSeed, ctx.startRoom);
	gameplayScene->runStat = ctx.runStat;
	
	for (int i = 0; i < ctx.playerCount; ++i) {
		gameplayScene->AddPlayer(
			ctx.playerSave[i].type,
			ctx.playerSave[i].pos,
			ctx.playerSave[i].curHealth,
			ctx.playerSave[i].selfDamageTime,
			ctx.playerSave[i].selfDamageCounter,
			ctx.playerSave[i].keyCount
			);
	}

	nextScene = std::move(gameplayScene);
	curState = ctx.playerCount == 1 ? SceneManagerState::SingleScene : SceneManagerState::MultiScene;

	return true;
}
