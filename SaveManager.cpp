#include "SaveManager.h"
#include <fstream>
#include "SmallStructs.h"
#include <string>

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

bool SaveManager::LoadFile(const std::string path, SaveContext& saveCtx) {
	std::ifstream saveFile(path);

	if (!saveFile.is_open()) {
		return false;
	}
	
	SaveContext tmpCtx;
	List<RoomSave> loadedRooms;

	saveFile >> tmpCtx.roomSeed;
	saveFile >> tmpCtx.startRoom.x >> tmpCtx.startRoom.y;
	saveFile >> tmpCtx.runStat.playerCount >> tmpCtx.runStat.totalEnemyKilled >> tmpCtx.runStat.totalTime;
	saveFile >> tmpCtx.playerCount;

	for (int i = 0; i < 2; ++i) {
		int alive = 0;
		std::string type;

		saveFile >> alive;
		saveFile >> type;
		saveFile >> tmpCtx.playerSave[i].pos.x >> tmpCtx.playerSave[i].pos.y;
		saveFile >> tmpCtx.playerSave[i].curHealth;
		saveFile >> tmpCtx.playerSave[i].selfDamageTime;
		saveFile >> tmpCtx.playerSave[i].selfDamageCounter;
		saveFile >> tmpCtx.playerSave[i].keyCount;

		tmpCtx.playerSave[i].isAlive = alive == 1;
		if (type == "Swordsman") {
			tmpCtx.playerSave[i].type = PlayerType::Swordsman;
		}
		else if (type == "Archer") {
			tmpCtx.playerSave[i].type = PlayerType::Archer;
		}
		else {
			tmpCtx.playerSave[i].type = PlayerType::Wizard;
		}
	}

	int x = 0, y = 0;
	int completed[4] = { 0, 0, 0, 0 };
	while (saveFile >> x >> y >> completed[0] >> completed[1] >> completed[2] >> completed[3]) {
		RoomSave tmpRoomSave;
		tmpRoomSave.coord = { x, y };
		tmpRoomSave.completed[0] = completed[0] == 1;
		tmpRoomSave.completed[1] = completed[1] == 1;
		tmpRoomSave.completed[2] = completed[2] == 1;
		tmpRoomSave.completed[3] = completed[3] == 1;
		loadedRooms.PushBack(tmpRoomSave);
	}

	roomSaves = std::move(loadedRooms);
	saveCtx = tmpCtx;

	return true;
}

void SaveManager::SaveFile(const std::string path, const SaveContext& saveCtx) {
	std::ofstream saveFile(path);
	std::string sav = saveCtx.ToString();
	for (int i = 0; i < roomSaves.GetSize(); ++i) {
		sav += roomSaves[i].ToString();
	}

	saveFile << sav;
}
