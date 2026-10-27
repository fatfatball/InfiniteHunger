#include "Room0.h"

std::string Room0::GetTileFolderPath() {
	return "./Resources/Tiles";
}

std::string Room0::GetMapFolderPath() {
	return "./Resources/Maps/Room0";
}

Room0::Room0(GamesEngineeringBase::Window& canvas) : Tilemap(canvas){

}
