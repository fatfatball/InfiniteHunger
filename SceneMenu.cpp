#include "SceneMenu.h"
#include "ClickableText.h"
#include <memory>
#include "SmallStructs.h"
#include "SceneManager.h"

SceneMenu::SceneMenu(GamesEngineeringBase::Window& window, Camera& camera, SceneManager* manager) 
	:Scene(window, camera, manager) {	
	StartMainMenu();
	player1 = PlayerType::Swordsman;
	player2 = PlayerType::Swordsman;
}

void SceneMenu::StartMainMenu() {
	for (int i = 0; i < uiObjects.GetSize(); ++i) {
		uiObjects[i]->isPendingDestroy = true;
	}

	AddUIObject(std::make_shared<ClickableText>(canvas, "INFINITE HUNGER", Vector2(200, 100), 7));
	AddUIObject(std::make_shared<ClickableText>(canvas, "START TUTORIAL", Vector2(50, 325), 5,
		[this]() {
			// TODO: implement tutorial scene
		}));
	AddUIObject(std::make_shared<ClickableText>(canvas, "START SINGLE PLAYER", Vector2(50, 400), 5,
		[this]() {
			StartPlayerSelection(1);
		}));
	AddUIObject(std::make_shared<ClickableText>(canvas, "START MULTIPLAYER", Vector2(50, 475), 5,
		[this]() {
			StartPlayerSelection(2);
		}));
	AddUIObject(std::make_shared<ClickableText>(canvas, "CONTINUE", Vector2(50, 550), 5,
		[this]() {
			// TODO: implement load game
		}));
	AddUIObject(std::make_shared<ClickableText>(canvas, "QUIT", Vector2(50, 625), 5,
		[this]() {
			sceneManager->QuitGame();
		}));
}

void SceneMenu::StartPlayerSelection(const int& count) {
	// clear the scene, then add the player selection UI
	for (int i = 0; i < uiObjects.GetSize(); ++i) {
		uiObjects[i]->isPendingDestroy = true;
	}

	AddUIObject(std::make_shared<ClickableText>(canvas, "PLAYER SELECTION", Vector2(200, 100), 6));
	AddUIObject(std::make_shared<ClickableText>(canvas, "START", Vector2(50, 550), 5,
		[this, count]() {
			sceneManager->ChangeScene(
				{ count == 1 ? SceneManagerState::SingleScene : SceneManagerState::MultiScene,
				player1, player2, {count, 0, 0.0} });
		}));
	AddUIObject(std::make_shared<ClickableText>(canvas, "BACK", Vector2(50, 625), 5, 
		[this]() {
			StartMainMenu();
		}));
	
	AddUIObject(std::make_shared<ClickableText>(canvas, "PLAYER 1: ", Vector2(50, 250), 5));
	auto player1Ptr = std::make_shared<ClickableText>(canvas, "SWORDSMAN", Vector2(350, 250), 5);
	AddUIObject(player1Ptr);
	AddUIObject(std::make_shared<ClickableText>(canvas, "SWORDSMAN", Vector2(50, 325), 4,
		[this, player1Ptr]() {
			player1 = PlayerType::Swordsman;
			player1Ptr->SetText("SWORDSMAN");
		}));
	AddUIObject(std::make_shared<ClickableText>(canvas, "ARCHER", Vector2(325, 325), 4,
		[this, player1Ptr]() {
			player1 = PlayerType::Archer;
			player1Ptr->SetText("ARCHER");
		}));
	AddUIObject(std::make_shared<ClickableText>(canvas, "WIZARD", Vector2(525, 325), 4,
		[this, player1Ptr]() {
			player1 = PlayerType::Wizard;
			player1Ptr->SetText("WIZARD");
		}));

	if (count == 2) {
		AddUIObject(std::make_shared<ClickableText>(canvas, "PLAYER 2: ", Vector2(50, 400), 5));
		auto player2Ptr = std::make_shared<ClickableText>(canvas, "SWORDSMAN", Vector2(350, 400), 5);
		AddUIObject(player2Ptr);
		AddUIObject(std::make_shared<ClickableText>(canvas, "SWORDSMAN", Vector2(50, 475), 4,
			[this, player2Ptr]() {
				player2 = PlayerType::Swordsman;
				player2Ptr->SetText("SWORDSMAN");
			}));
		AddUIObject(std::make_shared<ClickableText>(canvas, "ARCHER", Vector2(325, 475), 4,
			[this, player2Ptr]() {
				player2 = PlayerType::Archer;
				player2Ptr->SetText("ARCHER");
			}));
		AddUIObject(std::make_shared<ClickableText>(canvas, "WIZARD", Vector2(525, 475), 4,
			[this, player2Ptr]() {
				player2 = PlayerType::Wizard;
				player2Ptr->SetText("WIZARD");
			}));
	}
}
