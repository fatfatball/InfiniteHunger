#include "Room0.h"
#include "EnemyMedium.h"
#include "Vector2.h"
#include "Scene.h"
#include <memory>
#include "ItemTrap.h"
#include "ItemTrapTrigger.h"
#include "ItemKey.h"
#include "SaveManager.h"

Room0::Room0(GamesEngineeringBase::Window& canvas, const RoomCoord coord)
	: Room(canvas, coord, "./Resources/Maps/Room0") {
	globalPosition = Vector2{ (float)coord.x * ROOM_WIDTH * TILE_LENGTH, (float)coord.y * ROOM_HEIGHT * TILE_LENGTH };
	curState.globalPositionS = globalPosition;
	prevState.globalPositionS = globalPosition;
}

void Room0::OnStart() {
	Room::OnStart();
	// load here
	Vector2 pos;
	SaveManager* manager = saveManager;
	RoomCoord roomCoord = coord;

	pos = { 64 * 8, -64 * 8 };
	auto ptr_trap3 = std::make_shared<ItemTrap>(env, pos + globalPosition);
	scene->AddObject(ptr_trap3);
	roomObjects.PushBack(ptr_trap3);

	pos = { -64 * 8, 64 * 8 };
	auto ptr_trap4 = std::make_shared<ItemTrap>(env, pos + globalPosition);
	scene->AddObject(ptr_trap4);
	roomObjects.PushBack(ptr_trap4);

	pos = { -64*8, -64 * 8 };
	auto ptr_trap0 = std::make_shared<ItemTrapTrigger>(env, pos + globalPosition, ptr_trap3);
	ptr_trap3->SetProjectileDirection((ptr_trap0->globalPosition - ptr_trap3->globalPosition).Normalized());
	scene->AddObject(ptr_trap0);
	roomObjects.PushBack(ptr_trap0);

	if (!manager->IsCompleted(roomCoord, 0)) {
		auto ptr0 = std::make_shared<ItemKey>(env, pos + globalPosition);
		ptr0->onSaving = [manager, roomCoord]() {
			manager->MarkCompleted(roomCoord, 0);
			};
		scene->AddObject(ptr0);
		roomObjects.PushBack(ptr0);
	}

	pos = { 64 * 8, 64 * 8 };
	auto ptr_trap1 = std::make_shared<ItemTrapTrigger>(env, pos + globalPosition, ptr_trap4);
	ptr_trap4->SetProjectileDirection((ptr_trap1->globalPosition - ptr_trap4->globalPosition).Normalized());
	scene->AddObject(ptr_trap1);
	roomObjects.PushBack(ptr_trap1);

	if (!manager->IsCompleted(roomCoord, 1)) {
		auto ptr1 = std::make_shared<ItemKey>(env, pos + globalPosition);
		ptr1->onSaving = [manager, roomCoord]() {
			manager->MarkCompleted(roomCoord, 1);
			};
		scene->AddObject(ptr1);
		roomObjects.PushBack(ptr1);
	}

	if (!manager->IsCompleted(roomCoord, 2)) {
		pos = { 64 * 5, 64 * 5 };
		auto ptr3 = std::make_shared<EnemyMedium>(env, pos + globalPosition);
		ptr3->onSaving = [manager, roomCoord]() {
			manager->MarkCompleted(roomCoord, 2);
			};
		scene->AddObject(ptr3);
	}

	if (!manager->IsCompleted(roomCoord, 3)) {
		pos = { -64 * 5, -64 * 5 };
		auto ptr4 = std::make_shared<EnemyMedium>(env, pos + globalPosition);
		ptr4->onSaving = [manager, roomCoord]() {
			manager->MarkCompleted(roomCoord, 3);
			};
		scene->AddObject(ptr4);
	}
}