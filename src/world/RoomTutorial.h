#pragma once
#include "Room.h"
#include "GamesEngineeringBase.h"

class RoomTutorial : public Room {
public:
	RoomTutorial(GamesEngineeringBase::Window& canvas, const RoomCoord coord);

	void OnStart() override; // load enemies
};

