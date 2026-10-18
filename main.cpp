#include "GamesEngineeringBase.h"
#include "Constants.h"
#include "TestGameObject.h"

int main() {
	GamesEngineeringBase::Window canvas;
	canvas.create(1280, 720, "test_window_name");
	bool running = true;
	GamesEngineeringBase::Timer loopTimer = GamesEngineeringBase::Timer();
	float frameTime;
	float frameTimeAccumulator = 0;
	TestGameObject* objPtr = new TestGameObject(canvas);

	while (running) {
		frameTime = loopTimer.dt();
		if (frameTime > 0.1f) {
			frameTime = 0.1f; // to avoid enormous dt
		}
		frameTimeAccumulator += frameTime;

#pragma region FixedUpdate
		while (frameTimeAccumulator > FIXED_DELTATIME) {
			// TODO: traverse the fixed_updated objects
			// TODO: store the previous state
			// TODO: update the current state using FIXED_DELTATIME

			frameTimeAccumulator -= FIXED_DELTATIME;
		}
#pragma endregion

#pragma region VisualUpdate
		// TODO: traverse the objects, update them by frameTime
		// for fixed-updated objects, use intersection: 
		//	alpha = frameTimeAccumulator / FIXED_DELTATIME
		//	renderState = prevState * (1.0 - alpha) + curState * alpha
		// for visual-updated objects, just update
#pragma endregion

#pragma region Render
		canvas.clear();
		// TODO: traverse the render-able objects, sort them by order, 
		// then call render method respectively.
		canvas.present();
#pragma endregion

#pragma region RecycleMarkedObjects
		// TODO: traverse the recycle-marked objects, delete them all
#pragma endregion

		// only for test, esc to close
		if (canvas.keyPressed(0x1b)) {
			running = false;
		}
	}
	
	if (objPtr != nullptr) {
		delete objPtr;
		objPtr = nullptr;
	}
	return 0;
}