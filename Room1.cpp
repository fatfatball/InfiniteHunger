#include "Room1.h"
#include "ItemLockedDoor.h"
#include <memory>
#include "Vector2.h"
#include "Scene.h"
#include "EnemySlow.h"

Room1::Room1(GamesEngineeringBase::Window& canvas, const RoomCoord coord)
	: Room(canvas, coord, "./Resources/Maps/Room1") {
	globalPosition = Vector2{ (float)coord.x * ROOM_WIDTH * TILE_LENGTH, (float)coord.y * ROOM_HEIGHT * TILE_LENGTH };
	curState.globalPositionS = globalPosition;
	prevState.globalPositionS = globalPosition;
}

void Room1::OnStart() {
	Room::OnStart();
	// load here
	Vector2 pos;
	SaveManager* manager = saveManager;
	RoomCoord roomCoord = coord;

	if (!manager->IsCompleted(roomCoord, 0)) {
		pos = { 64 * 2, 64 * -4 };
		auto ptr0 = std::make_shared<ItemLockedDoor>(env, pos + globalPosition);
		ptr0->onSaving = [manager, roomCoord]() {
			manager->MarkCompleted(roomCoord, 0);
			};
		scene->AddObject(ptr0);
		roomObjects.PushBack(ptr0);
	}

	if (!manager->IsCompleted(roomCoord, 1)) {
		pos = { 64 * 4, 64 * 2 };
		auto ptr1 = std::make_shared<ItemLockedDoor>(env, pos + globalPosition);
		ptr1->onSaving = [manager, roomCoord]() {
			manager->MarkCompleted(roomCoord, 1);
			};
		scene->AddObject(ptr1);
		roomObjects.PushBack(ptr1);
	}

	if (!manager->IsCompleted(roomCoord, 2)) {
		pos = { 64 * 8, 64 * 8 };
		auto ptr2 = std::make_shared<EnemySlow>(env, pos + globalPosition);
		ptr2->onSaving = [manager, roomCoord]() {
			manager->MarkCompleted(roomCoord, 2);
			};
		ptr2->onUnsave = [manager, roomCoord]() {
			manager->UndoCompleted(roomCoord, 2);
			};
		scene->AddObject(ptr2);
	}

	if (!manager->IsCompleted(roomCoord, 3)) {
		pos = { 64 * 8, 64 * -8 };
		auto ptr3 = std::make_shared<EnemySlow>(env, pos + globalPosition);
		ptr3->onSaving = [manager, roomCoord]() {
			manager->MarkCompleted(roomCoord, 3);
			};
		ptr3->onUnsave = [manager, roomCoord]() {
			manager->UndoCompleted(roomCoord, 3);
			};
		scene->AddObject(ptr3);
	}
}
