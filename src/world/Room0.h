#pragma once
#include "Room.h"
#include "GamesEngineeringBase.h"

class Room0 : public Room {
public:
	Room0(GamesEngineeringBase::Window& canvas, const RoomCoord coord);

	void OnStart() override; // load enemies
};