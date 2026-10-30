#include "Room0.h"
#include <string>
#include "EnemyFast.h"
#include "Vector2.h"
#include "Scene.h"
#include <memory>

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
	pos = { 80, 80 };
	scene->AddObject(std::make_shared<EnemyFast>(env, pos + globalPosition));
}