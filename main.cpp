#include "GamesEngineeringBase.h"
#include "Constants.h"
#include "TestGameObject.h"
#include "Camera.h"
#include <iostream>
#include "List.h"
#include "Scene.h"
#include <memory>

int main() {
	GamesEngineeringBase::Window canvas;
	canvas.create(1280, 720, "test_window_name");
	bool running = true;
	Camera camera;

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

	Scene mainScene = Scene(canvas, camera);
	mainScene.AddObject(std::make_shared<TestGameObject>(canvas));
	bool prevBPressed = false;

	while (running) {
		frameTime = loopTimer.dt();

#pragma region PerformanceDetect
		frameTimeCounter += frameTime;
		if (frameTimeCounter > 1.0f) {
			std::cout << "FPS: " << frameCounter / frameTimeCounter << std::endl;
			std::cout << "fixed update time: " << fixedUpdateTime << std::endl;
			std::cout << "clear time: " << clearTime << std::endl;
			std::cout << "render time: " << renderTime << std::endl;
			std::cout << "present time: " << presentTime << std::endl;
			frameTimeCounter = 0;
			frameCounter = 0;
			fixedUpdateTime = 0;
			clearTime = 0;
			renderTime = 0;
			presentTime = 0;
		}
		frameCounter += 1;
#pragma endregion

		if (frameTime > 0.1f) {
			frameTime = 0.1f; // to avoid enormous dt
		}
		frameTimeAccumulator += frameTime;

#pragma region FixedUpdate
		performanceTestTimer.reset();
		while (frameTimeAccumulator >= FIXED_DELTATIME) {
			// TODO: traverse the fixed_updated objects
			// TODO: store the previous state
			// TODO: update the current state using FIXED_DELTATIME
			// TODO: traverse the recycle-marked objects, delete them all
			mainScene.FixedUpdateObjects(FIXED_DELTATIME);
			mainScene.RecycleObjects();
			frameTimeAccumulator -= FIXED_DELTATIME;
		}
		fixedUpdateTime += performanceTestTimer.dt();
#pragma endregion

#pragma region VisualUpdate
		// TODO: traverse the objects, update them by frameTime
		// for fixed-updated objects, use interpolation: 
		//	alpha = frameTimeAccumulator / FIXED_DELTATIME
		//	renderState = prevState * (1.0 - alpha) + curState * alpha
		// for visual-updated objects, just update
		mainScene.VisualUpdateObjects(frameTime);
#pragma endregion

#pragma region Render
		performanceTestTimer.reset();
		canvas.clear();
		clearTime += performanceTestTimer.dt();
		// TODO: traverse the render-able objects, sort them by order, 
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