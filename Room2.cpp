#include "Room2.h"
#include "EnemyStatic.h"
#include "EnemyFast.h"
#include "EnemyMedium.h"
#include "ItemKey.h"
#include <memory>
#include "Vector2.h"
#include "Scene.h"

Room2::Room2(GamesEngineeringBase::Window& canvas, const RoomCoord coord)
	: Room(canvas, coord, "./Resources/Maps/Room2") {
	globalPosition = Vector2{ (float)coord.x * ROOM_WIDTH * TILE_LENGTH, (float)coord.y * ROOM_HEIGHT * TILE_LENGTH };
	curState.globalPositionS = globalPosition;
	prevState.globalPositionS = globalPosition;
}

void Room2::OnStart() {
	Room::OnStart();

	Vector2 pos;
	SaveManager* manager = saveManager;
	RoomCoord roomCoord = coord;

	if (!manager->IsCompleted(roomCoord, 0)) {
		pos = { 64 * 7, 64 * -5 };
		auto ptr0 = std::make_shared<EnemyFast>(env, pos + globalPosition);
		ptr0->onSaving = [manager, roomCoord]() {
			manager->MarkCompleted(roomCoord, 0);
			};
		scene->AddObject(ptr0);
	}

	if (!manager->IsCompleted(roomCoord, 1)) {
		pos = { 64 * 7, 64 * 7 };
		auto ptr1 = std::make_shared<EnemyStatic>(env, pos + globalPosition);
		ptr1->onSaving = [manager, roomCoord]() {
			manager->MarkCompleted(roomCoord, 1);
			};
		scene->AddObject(ptr1);
	}

	if (!manager->IsCompleted(roomCoord, 2)) {
		pos = { 64 * -7, 64 * 6 };
		auto ptr2 = std::make_shared<EnemyMedium>(env, pos + globalPosition);
		ptr2->onSaving = [manager, roomCoord]() {
			manager->MarkCompleted(roomCoord, 2);
			};
		scene->AddObject(ptr2);
	}

	if (!manager->IsCompleted(roomCoord, 3)) {
		pos = { 64 * -7, 64 * -7 };
		auto ptr3 = std::make_shared<ItemKey>(env, pos + globalPosition);
		ptr3->onSaving = [manager, roomCoord]() {
			manager->MarkCompleted(roomCoord, 3);
			};
		scene->AddObject(ptr3);
		roomObjects.PushBack(ptr3);
	}
}

