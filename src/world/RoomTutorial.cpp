#include "RoomTutorial.h"
#include "ItemKey.h"
#include "ItemLockedDoor.h"
#include "Scene.h"
#include "EnemySlow.h"
#include "EnemyFast.h"
#include "EnemyMedium.h"
#include "EnemyStatic.h"

RoomTutorial::RoomTutorial(GamesEngineeringBase::Window& canvas, const RoomCoord coord)
	: Room(canvas, coord, "./Resources/Maps/RoomTutorial") {
	globalPosition = Vector2{ (float)coord.x * ROOM_WIDTH * TILE_LENGTH, (float)coord.y * ROOM_HEIGHT * TILE_LENGTH };
	curState.globalPositionS = globalPosition;
	prevState.globalPositionS = globalPosition;
}

void RoomTutorial::OnStart() {
	Room::OnStart();
	Vector2 pos;
	SaveManager* manager = saveManager;
	RoomCoord roomCoord = coord;

	{
		pos = { 64 * 8, 64 * 8 };
		auto ptr = std::make_shared<ItemKey>(env, pos + globalPosition);
		scene->AddObject(ptr);
		roomObjects.PushBack(ptr);
	}
	{
		pos = { -64 * 8, 64 * 8 };
		auto ptr = std::make_shared<ItemKey>(env, pos + globalPosition);
		scene->AddObject(ptr);
		roomObjects.PushBack(ptr);
	}
	{
		pos = { 64 * 8, -64 * 8 };
		auto ptr = std::make_shared<ItemKey>(env, pos + globalPosition);
		scene->AddObject(ptr);
		roomObjects.PushBack(ptr);
	}
	{
		pos = { -64 * 8, -64 * 8 };
		auto ptr = std::make_shared<ItemKey>(env, pos + globalPosition);
		scene->AddObject(ptr);
		roomObjects.PushBack(ptr);
	}
	{
		pos = { 64 * 3, 64 * 0 };
		auto ptr = std::make_shared<ItemLockedDoor>(env, pos + globalPosition);
		scene->AddObject(ptr);
		roomObjects.PushBack(ptr);
	}
	{
		pos = { 64 * 0, 64 * 3 };
		auto ptr = std::make_shared<ItemLockedDoor>(env, pos + globalPosition);
		scene->AddObject(ptr);
		roomObjects.PushBack(ptr);
	}
	{
		pos = { -64 * 3, 64 * 0 };
		auto ptr = std::make_shared<ItemLockedDoor>(env, pos + globalPosition);
		scene->AddObject(ptr);
		roomObjects.PushBack(ptr);
	}
	{
		pos = { 64 * 0, -64 * 3 };
		auto ptr = std::make_shared<ItemLockedDoor>(env, pos + globalPosition);
		scene->AddObject(ptr);
		roomObjects.PushBack(ptr);
	}

	{
		pos = { 64 * 8, 64 * 0 };
		auto ptr = std::make_shared<EnemyFast>(env, pos + globalPosition);
		scene->AddObject(ptr);
	}
	{
		pos = { 64 * 0, 64 * 8 };
		auto ptr = std::make_shared<EnemyMedium>(env, pos + globalPosition);
		scene->AddObject(ptr);
	}
	{
		pos = { -64 * 8, 64 * 0 };
		auto ptr = std::make_shared<EnemySlow>(env, pos + globalPosition);
		scene->AddObject(ptr);
	}
	{
		pos = { 64 * 0, -64 * 8 };
		auto ptr = std::make_shared<EnemyStatic>(env, pos + globalPosition);
		scene->AddObject(ptr);
	}
}
