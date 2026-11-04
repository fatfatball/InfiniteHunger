#include "SaveManager.h"

int SaveManager::FindIndex(const RoomCoord& coord) {
	for (int i = 0; i < roomSaves.GetSize(); ++i) {
		if (roomSaves[i].coord.x == coord.x && roomSaves[i].coord.y == coord.y) {
			return i;
		}
	}

	return -1;
}

bool SaveManager::IsCompleted(const RoomCoord& coord, const int& slot) {
	int ind = FindIndex(coord);
	if (ind < 0) {
		return false;
	}

	return roomSaves[ind].completed[slot];
}

void SaveManager::MarkCompleted(const RoomCoord& coord, const int& slot) {
	int ind = FindIndex(coord);
	if (ind < 0) {
		// create a new roomsave
		RoomSave save{ coord };
		save.completed[slot] = true;
		roomSaves.PushBack(save);
		return;
	}
	else {
		roomSaves[ind].completed[slot] = true;
	}
}

void SaveManager::UndoCompleted(const RoomCoord& coord, const int& slot) {
	int ind = FindIndex(coord);
	if (ind < 0) { return; }
	roomSaves[ind].completed[slot] = false;
}

void SaveManager::Clear() {
	roomSaves.Clear();
}

bool SaveManager::LoadFile(const std::string path)
{
	return false;
}

void SaveManager::SaveFile(const std::string path)
{}
