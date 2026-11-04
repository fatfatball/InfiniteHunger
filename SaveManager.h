#pragma once
#include "List.h"
#include "SmallStructs.h"
#include <string>

class SaveManager {
private:
	List<RoomSave> roomSaves;

	int FindIndex(const RoomCoord& coord);

public:
	bool IsCompleted(const RoomCoord& coord, const int& slot);
	void MarkCompleted(const RoomCoord& coord, const int& slot);
	void Clear();
	// TODO
	bool LoadFile(const std::string path);
	void SaveFile(const std::string path);
};

