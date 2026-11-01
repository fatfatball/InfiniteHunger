#include "Room0.h"
#include <string>
#include "EnemyFast.h"
#include "EnemyMedium.h"
#include "EnemySlow.h"
#include "EnemyStatic.h"
#include "Vector2.h"
#include "Scene.h"
#include <memory>
#include "ItemKey.h"
#include "ItemLockedDoor.h"

Room0::Room0(GamesEngineeringBase::Window& canvas, const RoomCoord coord)
	: Room(canvas, coord, "./Resources/Maps/Room0") {
	globalPosition = Vector2{ (float)coord.x * ROOM_WIDTH * TILE_LENGTH, (float)coord.y * ROOM_HEIGHT * TILE_LENGTH };
	curState.globalPositionS = globalPosition;
	prevState.globalPositionS = globalPosition;
}

void Room0::OnStart() {
	Room::OnStart();
	// load enemies here
	Vector2 pos;
	pos = { 128, 128 };
	scene->AddObject(std::make_shared<EnemyStatic>(env, pos + globalPosition));
	scene->AddObject(std::make_shared<ItemKey>(env, pos + globalPosition));
	pos = { 512, 128 };
	scene->AddObject(std::make_shared<ItemLockedDoor>(env, pos + globalPosition));

}