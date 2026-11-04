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
	void UndoCompleted(const RoomCoord& coord, const int& slot);
	void Clear();
	// TODO
	// return the saveContext by saveCtx
	bool LoadFile(const std::string path, SaveContext& saveCtx);
	void SaveFile(const std::string path, const SaveContext& saveCtx);
};

