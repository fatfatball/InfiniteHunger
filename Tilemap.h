#pragma once
#include "BaseGameObject.h"

// store (and order) the tiles
// draw them
// manage them

// each tile would be a room of 1500*1500 (world axis), all corners blocked, like Issac rooms
class Tilemap : public BaseGameObject {
public:
	// read the tile images, instanciate the tiles around the camera
	void OnStart() override;
	void FixedUpdate(const float& dt) override;
	// monitor the camera's position, always keep the 5 tiles around are instanciated
	// and destroy other tiles.
	// also, change the boundary of camera in real time.
	// all spawned tiles (and the enemies) should be recorded
	void VisualUpdate(const float& dt) override;
	void OnDestroy() override;
};

