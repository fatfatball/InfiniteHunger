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

	GamesEngineeringBase::Timer loopTimer = GamesEngineeringBase::Timer();
	float frameTime;
	float frameTimeAccumulator = 0;
	int frameCounter = 0;
	float frameTimeCounter = 0;

	Scene mainScene = Scene(canvas, camera);
	mainScene.AddObject(std::make_shared<TestGameObject>(canvas));
	bool prevBPressed = false;

	while (running) {
		frameTime = loopTimer.dt();

		frameTimeCounter += frameTime;
		if (frameTimeCounter > 0.5f) {
			std::cout << "FPS: " << frameCounter / frameTimeCounter << std::endl;
			frameTimeCounter = 0;
			frameCounter = 0;
		}
		frameCounter += 1;

		if (frameTime > 0.1f) {
			frameTime = 0.1f; // to avoid enormous dt
		}
		frameTimeAccumulator += frameTime;

#pragma region FixedUpdate
		while (frameTimeAccumulator >= FIXED_DELTATIME) {
			// TODO: traverse the fixed_updated objects
			// TODO: store the previous state
			// TODO: update the current state using FIXED_DELTATIME
			// TODO: traverse the recycle-marked objects, delete them all
			mainScene.FixedUpdateObjects(FIXED_DELTATIME);
			mainScene.RecycleObjects();
			frameTimeAccumulator -= FIXED_DELTATIME;
		}
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
		canvas.clear();
		// TODO: traverse the render-able objects, sort them by order, 
		// then call render method respectively.
		mainScene.RenderObjects(frameTimeAccumulator / FIXED_DELTATIME);
		canvas.present();
#pragma endregion

		// only for test
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
	}
	return 0;
}