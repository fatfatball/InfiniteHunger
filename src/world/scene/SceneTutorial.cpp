#include "SceneTutorial.h"
#include "Scene.h"
#include <memory>
#include "RoomTutorial.h"
#include "Smallstructs.h"
#include "ObjectTimer.h"
#include "SceneManager.h"

SceneTutorial::SceneTutorial(GamesEngineeringBase::Window& window, Camera& camera,
	SceneManager* manager, SaveManager* saveManager) : Scene(window, camera, manager) {
	roomManager = std::make_shared<RoomManager>(canvas, saveManager);
	roomManager->scene = this;
	roomManager->isInfinite = false;
	AddObject(roomManager);
	s = saveManager;
	auto ptr = std::make_shared<RoomTutorial>(window, RoomCoord{ 0,0 });
	roomManager->AddRoom(ptr);
	AddObject(ptr);
	SetCameraBoundary(roomManager->GetRoomAt(RoomCoord{ 0,0 })->GetAABB());

	AddUIObject(std::make_shared<ClickableText>(canvas, "BACK TO MENU", Vector2(50, 50), 5,
		[this]() {
			sceneManager->ChangeScene({ SceneManagerState::MenuScene });
		}, ColorRGBA{ 100, 100, 100, 255 }));
	AddUIObject(std::make_shared<ClickableText>(canvas,
		"1. MOVE: W A S D (P1) / UP DOWN LEFT RIGHT (P2)", Vector2(50, 125), 2, ColorRGBA{ 150, 150, 150, 255 }));
	AddUIObject(std::make_shared<ClickableText>(canvas,
		"2. ATTACK: C (P1) / , (P2) SKILL V (P1) / . (P2)", Vector2(50, 175), 2, ColorRGBA{ 150, 150, 150, 255 }));
	AddUIObject(std::make_shared<ClickableText>(canvas,
		"3. SAVE AND QUIT: ESC", Vector2(50, 225), 2, ColorRGBA{ 150, 150, 150, 255 }));
	AddUIObject(std::make_shared<ClickableText>(canvas,
		"4. YOU FEEL HUNGRY FROM TIME TO TIME", Vector2(50, 275), 2, ColorRGBA{ 150, 150, 150, 255 }));
	AddUIObject(std::make_shared<ClickableText>(canvas,
		"5. FIGHT, EAT, AND LIVE AS LONG AS YOU CAN", Vector2(50, 325), 2, ColorRGBA{ 150, 150, 150, 255 }));
}

void SceneTutorial::PlayerAllDied() {
	// wait 3 seconds, then go to menu scene
	auto ptr = std::make_shared<ObjectTimer>(canvas);
	ptr->SetTimer(3.f, [this]() {
		sceneManager->ChangeScene({ SceneManagerState::MenuScene });
		});
	ptr->StartTimer();
	AddObject(ptr);
}
