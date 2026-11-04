#pragma once
#include "Room.h"

class Room1 : public Room {
public:
	Room1(GamesEngineeringBase::Window& canvas, const RoomCoord coord);

	void OnStart() override; // load enemies
};

