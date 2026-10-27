#pragma once
#include "Tilemap.h"
#include "GamesEngineeringBase.h"

class Room0 : public Tilemap{
private:
	std::string GetTileFolderPath() override;
	std::string GetMapFolderPath() override;
public:
	Room0(GamesEngineeringBase::Window& canvas);
};

