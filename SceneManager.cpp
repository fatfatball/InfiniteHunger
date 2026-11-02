#include "SceneManager.h"
#include "SceneGameplay.h"
#include <memory>
#include "TextRenderer.h"
#include "SceneMenu.h"

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
}

void SceneManager::ChangeScene(const SceneChangeContext& ctx) {
	camera.ResetCamera();
	switch (ctx.nextScene) {
	case SceneManagerState::None:
		break;
	case SceneManagerState::SingleScene:
		nextScene = std::make_unique<SceneGameplay>(canvas, camera, this);
		nextScene->AddPlayer(ctx.player1);
		break;
	case SceneManagerState::MultiScene:
		nextScene = std::make_unique<SceneGameplay>(canvas, camera, this);
		nextScene->AddPlayer(ctx.player1);
		nextScene->AddPlayer(ctx.player2);
		break;
	case SceneManagerState::LoadScene:
		break;
	case SceneManagerState::MenuScene:
		nextScene = std::make_unique<SceneMenu>(canvas, camera, this);
		break;
	case SceneManagerState::TallyScene:
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
}

void SceneManager::VisualUpdate(const float& dt) {
	if (!currentScene) {
		return;
	}
	currentScene->VisualUpdateObjects(dt);
	camera.VisualUpdate(dt);
}

void SceneManager::Render(const float& alpha, const int& fpsCount) {
	if (canvas.keyPressed(0x1b)) { // esc to close
		running = false;
	}

	if (!currentScene) {
		return;
	}
	canvas.clear();
	currentScene->RenderObjects(alpha);
	TextRenderer::DrawText(canvas, "FPS: " + std::to_string(fpsCount), 5, 5, 2, { 127, 127, 127, 255 });
	canvas.present();
	SolveSceneChange();
}
