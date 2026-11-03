#include "Room2.h"

Room2::Room2(GamesEngineeringBase::Window& canvas, const RoomCoord coord)
	: Room(canvas, coord, "./Resources/Maps/Room2") {
	globalPosition = Vector2{ (float)coord.x * ROOM_WIDTH * TILE_LENGTH, (float)coord.y * ROOM_HEIGHT * TILE_LENGTH };
	curState.globalPositionS = globalPosition;
	prevState.globalPositionS = globalPosition;
}

void Room2::OnStart() {
	Room::OnStart();
}

