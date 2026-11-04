#pragma once
#include "Scene.h"
#include "GamesEngineeringBase.h"
#include "Camera.h"
#include "SmallStructs.h"

class SceneMenu : public Scene {
private:
	PlayerType player1;
	PlayerType player2;

public:
	SceneMenu(GamesEngineeringBase::Window& window, Camera& camera, SceneManager* manager);
	
	void StartMainMenu();
	void StartPlayerSelection(const int& count);
};

