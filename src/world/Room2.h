#pragma once
#include "Room.h"

class Room2 : public Room {
public:
	Room2(GamesEngineeringBase::Window& canvas, const RoomCoord coord);

	void OnStart() override; // load enemies
};

