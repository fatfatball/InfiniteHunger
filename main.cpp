#include "GamesEngineeringBase.h"
#include "Constants.h"
#include "TestGameObject.h"
#include "Camera.h"
#include <iostream>
#include "List.h"
#include "Scene.h"
#include <memory>
#include "TestPhysicalObject.h"
#include "PolyLine2D.h"
#include "Room0.h"

int main() {
	GamesEngineeringBase::Window canvas;
	canvas.create(WINDOW_WIDTH, WINDOW_HEIGHT, "test_window_name");
	bool running = true;
	Camera camera;
	Scene mainScene = Scene(canvas, camera);
	//mainScene.AddObject(std::make_shared<TestPhysicalObject>(canvas));
	auto room0 = std::make_shared<Room0>(canvas);
	mainScene.AddObject(room0);
	mainScene.AddTilemap(std::move(room0));

#pragma region Timers
	GamesEngineeringBase::Timer loopTimer;
	float frameTime;
	float frameTimeAccumulator = 0;
	int frameCounter = 0;
	float frameTimeCounter = 0;

	GamesEngineeringBase::Timer performanceTestTimer;
	float fixedUpdateTime = 0;
	float clearTime = 0;
	float renderTime = 0;
	float presentTime = 0;
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

	bool prevBPressed = false;

	while (running) {
		frameTime = loopTimer.dt();

#pragma region PerformanceDetect
		frameTimeCounter += frameTime;
		if (frameTimeCounter > 1.0f) {
			std::cout << "FPS: " << frameCounter / frameTimeCounter << std::endl;
			std::cout << "fixed update time: " << fixedUpdateTime / frameTimeCounter << std::endl;
			std::cout << "clear time: " << clearTime / frameTimeCounter << std::endl;
			std::cout << "render time: " << renderTime / frameTimeCounter << std::endl;
			std::cout << "present time: " << presentTime / frameTimeCounter << std::endl;
			frameTimeCounter = 0;
			frameCounter = 0;
			fixedUpdateTime = 0;
			clearTime = 0;
			renderTime = 0;
			presentTime = 0;
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
			mainScene.FixedUpdateObjects(FIXED_DELTATIME);
			// traverse the recycle-marked objects, delete them all
			mainScene.RecycleObjects(); 
			frameTimeAccumulator -= FIXED_DELTATIME;
		}
		fixedUpdateTime += performanceTestTimer.dt();
#pragma endregion

#pragma region VisualUpdate
		// traverse the objects, update them by frameTime
		mainScene.VisualUpdateObjects(frameTime);
#pragma endregion

#pragma region Render
		performanceTestTimer.reset();
		canvas.clear();
		clearTime += performanceTestTimer.dt();
		// traverse the render-able objects, sort them by order, 
		// then call render method respectively.
		mainScene.RenderObjects(frameTimeAccumulator / FIXED_DELTATIME);
		renderTime += performanceTestTimer.dt();
		canvas.present();
		presentTime += performanceTestTimer.dt();
#pragma endregion

#pragma region OnlyForTest
		if (canvas.keyPressed(0x1b)) { // esc to close
			running = false;
		}
		if (canvas.keyPressed(0x42)) { // B to spawn another test object
			if (prevBPressed == false) {
				mainScene.AddObject(std::make_shared<TestGameObject>(canvas));
			}
			prevBPressed = true;
		}
		else {
			prevBPressed = false;
		}
#pragma endregion
	}
	return 0;
}