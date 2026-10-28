#pragma once
#include "Tilemap.h"
#include <string>
#include "SmallStructs.h"
#include "Vector2.h"
#include "Constants.h"

class Room : public Tilemap {
protected:
	std::string mapFolder;
	RoomCoord coord;

	std::string GetTileFolderPath() override {
		return "./Resources/Tiles";
	}
	std::string GetMapFolderPath() override {
		return mapFolder;
	}
public:
	//short focusChanged; // 0: no change; 1: false to true; -1: true to false

	Room(GamesEngineeringBase::Window& canvas, const RoomCoord coord, const std::string mapFolder)
		: Tilemap(canvas), mapFolder(mapFolder), coord(coord) {
		isFocused = false;

		globalPosition = Vector2{ (float)coord.x * ROOM_WIDTH * TILE_LENGTH, (float)coord.y * ROOM_HEIGHT * TILE_LENGTH };
		curState.globalPositionS = globalPosition;
		prevState.globalPositionS = globalPosition;
		//focusChanged = 0;
	}

	RoomCoord GetCoord() const {
		return coord;
	}

	void UpdateRenderState(const float& alpha, const Camera& camera) override {
		//if (!camera.followTarget) {
		//	isFocused = false;
		//}
		//else {
		//	isFocused = GetAABB().IsInside(camera.followTarget->globalPosition);
		//}
		isFocused = GetAABB().Intersects(camera.GetAABB());
		renderAABB = GetAABB().GetIntersectionAABB(camera.GetAABB());
		renderScreenAABB = camera.GetScreenAABB(renderAABB);
	}
};

