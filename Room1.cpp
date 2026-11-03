#include "Room1.h"

Room1::Room1(GamesEngineeringBase::Window& canvas, const RoomCoord coord)
	: Room(canvas, coord, "./Resources/Maps/Room1") {
	globalPosition = Vector2{ (float)coord.x * ROOM_WIDTH * TILE_LENGTH, (float)coord.y * ROOM_HEIGHT * TILE_LENGTH };
	curState.globalPositionS = globalPosition;
	prevState.globalPositionS = globalPosition;
}

void Room1::OnStart() {
	Room::OnStart();
}
