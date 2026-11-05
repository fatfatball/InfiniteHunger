#pragma once
#include "Tilemap.h"
#include <string>
#include "SmallStructs.h"
#include "Vector2.h"
#include "Constants.h"
#include <memory>
#include "List.h"
#include "BaseGameObject.h"
#include "SaveManager.h"

// base room class. it should be organized by a room manager.
class Room : public Tilemap {
protected:
	std::string mapFolder;
	RoomCoord coord;
	List<std::weak_ptr<BaseGameObject>> roomObjects; // to store and destroy the static objects in this room

	std::string GetTileFolderPath() override {
		return "./Resources/Tiles";
	}
	std::string GetMapFolderPath() override {
		return mapFolder;
	}

public:
	SaveManager* saveManager;

	Room(GamesEngineeringBase::Window& canvas, const RoomCoord coord, const std::string mapFolder)
		: Tilemap(canvas), mapFolder(mapFolder), coord(coord) {
		isFocused = false;

		globalPosition = Vector2{ (float)coord.x * ROOM_WIDTH * TILE_LENGTH, (float)coord.y * ROOM_HEIGHT * TILE_LENGTH };
		curState.globalPositionS = globalPosition;
		prevState.globalPositionS = globalPosition;
		canCollide = true;

		tileWidth = ROOM_WIDTH;
		tileHeight = ROOM_HEIGHT;
		tileIds.Resize(tileWidth * tileHeight);
		tileCollisions.Resize(tileWidth * tileHeight);
		for (int i = 0; i < tileHeight; ++i) {
			for (int j = 0; j < tileWidth; ++j) {
				GetTileIds(j, i) = 0;
				GetTileCollisions(j, i) = false;
			}
		}

		saveManager = nullptr;
	}

	~Room() {
		for (int i = 0; i < roomObjects.GetSize(); ++i) {
			auto obj = roomObjects[i].lock();
			if (obj) {		
				obj->isActive = false;
				obj->isPendingDestroy = true;
			}
		}
	}

	RoomCoord GetCoord() const {
		return coord;
	}

	void UpdateRenderState(const float& alpha, const Camera& camera) override {
		isFocused = GetAABB().Intersects(camera.GetAABB());
		renderAABB = GetAABB().GetIntersectionAABB(camera.GetAABB());
		renderScreenAABB = camera.GetScreenAABB(renderAABB);
	}

	void DestroyRoomObjects() {
		for (int i = 0; i < roomObjects.GetSize(); ++i) {
			auto obj = roomObjects[i].lock();
			if (obj) {
				obj->isActive = false;
				obj->isPendingDestroy = true;
			}
		}
		roomObjects.Clear();
	}
};

