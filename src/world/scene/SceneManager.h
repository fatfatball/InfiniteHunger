#pragma once
#include "Scene.h"
#include "GamesEngineeringBase.h"
#include <memory>
#include "SmallStructs.h"
#include "SaveManager.h"

enum class SceneManagerState {
	None,
	SingleScene,
	MultiScene,
	LoadScene,
	MenuScene,
	TallyScene,
	TutorialScene,
};

struct SceneChangeContext {
	SceneManagerState nextScene;
	PlayerType player1 = PlayerType::Swordsman;
	PlayerType player2 = PlayerType::Archer;
	RunStat stat;
};

class SceneManager {
private:
	std::unique_ptr<Scene> currentScene;
	std::unique_ptr<Scene> nextScene;
	Camera camera;
	GamesEngineeringBase::Window canvas;
	bool& running;
	RunStat runStat;
	SceneManagerState curState;
	bool isPendingQuit;

	void SolveSceneChange();

public:
	SaveManager saveManager;

	SceneManager(bool& running);
	
	void ChangeScene(const SceneChangeContext& ctx);
	void FixedUpdate(const float& dt);
	void VisualUpdate(const float& dt);
	void Render(const float& alpha, const int& fpsCount);

	void QuitGame();

	RunStat GetRunStat() const { return runStat; }
	bool ContinueGame(std::string path);
};

