#include "GamesEngineeringBase.h"
#include "Constants.h"
#include "TestGameObject.h"
#include "Camera.h"
#include "List.h"
#include "Scene.h"
#include <memory>
#include "Player0.h"
#include "PolyLine2D.h"
#include "Room0.h"
#include "SmallStructs.h"
#include "TextRenderer.h"
#include "SceneManager.h"

int main() {
	bool running = true;
	//GamesEngineeringBase::Window canvas;
	//canvas.create(WINDOW_WIDTH, WINDOW_HEIGHT, "Infinite Hunger");
	//Camera camera;
	//Scene mainScene = Scene(canvas, camera);
	//mainScene.AddPlayer(PlayerType::Swordsman);
	SceneManager sceneManager(running);

#pragma region Timers
	GamesEngineeringBase::Timer loopTimer;
	float frameTime;
	float frameTimeAccumulator = 0;
	int frameCounter = 0;
	float frameTimeCounter = 0;

	GamesEngineeringBase::Timer performanceTestTimer;
	//float updateTime = 0;
	//float clearTime = 0;
	//float renderTime = 0;
	//float presentTime = 0;
	int fpsCount = 0;
#pragma endregion

#pragma region TestingObjects
	// single-thread performance:
	// really a disaster when it's semi-transparent. as a ref, 13.3 fps
	// fps can reach 77 when it's not transparent, good!
	// ------
	// 4-thread performance:
	// semi-transparent: 48 fps
	// non-transparent: 219 fps
	// ------
	// 8-thread performance:
	// semi-transparent: 77 fps
	// non-transparent: 215 fps
	//for (int i = 0; i < 500; ++i) {
	//	auto obj = std::make_shared<TestGameObject>(canvas);
	//	obj->globalPosition = { rand() % 1024 - 512.f, rand() % 768 - 384.f };
	//	mainScene.AddObject(std::move(obj));
	//}
#pragma endregion

	while (running) {
		frameTime = loopTimer.dt();

#pragma region PerformanceDetect
		frameTimeCounter += frameTime;
		if (frameTimeCounter > 1.0f) {
			fpsCount = frameCounter / frameTimeCounter;
			//std::cout << "FPS: " << frameCounter / frameTimeCounter << std::endl;
			//std::cout << "update time: " << updateTime / frameTimeCounter << std::endl;
			//std::cout << "clear time: " << clearTime / frameTimeCounter << std::endl;
			//std::cout << "render time: " << renderTime / frameTimeCounter << std::endl;
			//std::cout << "present time: " << presentTime / frameTimeCounter << std::endl;
			//std::cout << "current objects: " << mainScene.GetObjectCount() << std::endl;
			frameTimeCounter = 0;
			frameCounter = 0;
			//updateTime = 0;
			//clearTime = 0;
			//renderTime = 0;
			//presentTime = 0;
		}
		frameCounter += 1;
#pragma endregion

		if (frameTime > 0.1f) { // to avoid enormous dt
			frameTime = 0.1f;
		}
		frameTimeAccumulator += frameTime;

#pragma region FixedUpdate
		performanceTestTimer.reset();
		while (frameTimeAccumulator >= FIXED_DELTATIME) {
			// traverse the fixed_updated objects
			// store the previous state
			// update the current state using FIXED_DELTATIME
			sceneManager.FixedUpdate(FIXED_DELTATIME);
			frameTimeAccumulator -= FIXED_DELTATIME;
		}
#pragma endregion

#pragma region VisualUpdate
		// traverse the objects, update them by frameTime
		sceneManager.VisualUpdate(frameTime);
		//updateTime += performanceTestTimer.dt();
#pragma endregion

#pragma region Render
		//clearTime += performanceTestTimer.dt();
		// traverse the render-able objects, sort them by order, 
		// then call render method respectively.
		sceneManager.Render(frameTimeAccumulator / FIXED_DELTATIME, fpsCount);
		//renderTime += performanceTestTimer.dt();
		//presentTime += performanceTestTimer.dt();
#pragma endregion
	}
	return 0;
}