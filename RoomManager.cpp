#include "RoomManager.h"
#include "Room0.h"
#include "Scene.h"
#include <cmath>
#include "SmallStructs.h"

RoomManager::RoomManager(GamesEngineeringBase::Window& canvas) : BaseGameObject(canvas) {
	scale = { 0, 0 };
	prevRoom = { 100, 100 };
}

void RoomManager::OnStart() {
	scene->AddRoom(GetRandomRoomAt({ 0, 0 }));
}

void RoomManager::FixedUpdate(const float& dt) {
}

void RoomManager::VisualUpdate(const float& dt) {
	// check if the current room changes
	RoomCoord curRoom = Wolrd2RoomCoord(scene->GetCameraTargetPos());
	if (prevRoom != curRoom) {
		scene->AddRoom(GetRandomRoomAt({ curRoom.x + 1, curRoom.y }));
		scene->AddRoom(GetRandomRoomAt({ curRoom.x - 1, curRoom.y }));
		scene->AddRoom(GetRandomRoomAt({ curRoom.x, curRoom.y + 1 }));
		scene->AddRoom(GetRandomRoomAt({ curRoom.x, curRoom.y - 1 }));

		RoomCoord tmpRoom = { prevRoom.x + 1, prevRoom.y };
		if (tmpRoom.DistanceTo(curRoom) > 1) {
			scene->RemoveRoom(tmpRoom);
		}
		tmpRoom = { prevRoom.x - 1, prevRoom.y };
		if (tmpRoom.DistanceTo(curRoom) > 1) {
			scene->RemoveRoom(tmpRoom);
		}
		tmpRoom = { prevRoom.x, prevRoom.y + 1 };
		if (tmpRoom.DistanceTo(curRoom) > 1) {
			scene->RemoveRoom(tmpRoom);
		}
		tmpRoom = { prevRoom.x, prevRoom.y - 1 };
		if (tmpRoom.DistanceTo(curRoom) > 1) {
			scene->RemoveRoom(tmpRoom);
		}

		scene->SetCameraBoundary(GetRoomAt(curRoom)->GetAABB());
	}

	prevRoom = curRoom;
}

void RoomManager::OnDestroy() {
}

ColorRGBA RoomManager::GetColorAt(const float& u, const float& v) const {
	return ColorRGBA();
}

unsigned char RoomManager::GetAlphaAt(const float& u, const float& v) const {
	return 0;
}

void RoomManager::AddRoom(std::shared_ptr<Room> room) {
	if (!room) {
		return;
	}
	// check that it is not added twice
	if (HasRoom(room->GetCoord())) {
		return;
	}
	rooms.PushBack(room);
}

void RoomManager::RemoveRoom(const RoomCoord coord) {
	// traverse the rooms, find the corresponding room, set to pending destroy
	for (int i = 0; i < rooms.GetSize(); ++i) {
		if (rooms[i]->GetCoord() == coord) {
			rooms[i]->isPendingDestroy = true;
			rooms[i]->isActive = false;
			rooms.Erase(i);
			return;
		}
	}
}

bool RoomManager::HasRoom(const RoomCoord& coord)
{
	for (int i = 0; i < rooms.GetSize(); ++i) {
		if (!rooms[i]->isActive || rooms[i]->isPendingDestroy) {
			continue;
		}
		if (rooms[i]->GetCoord() == coord) {
			return true;
		}
	}
	return false;
}

Room* RoomManager::GetRoomAt(const RoomCoord& coord)
{
	for (int i = 0; i < rooms.GetSize(); ++i) {
		if (!rooms[i]->isActive || rooms[i]->isPendingDestroy) {
			continue;
		}
		if (rooms[i]->GetCoord() == coord) {
			return rooms[i].get();
		}
	}
	return nullptr;
}

std::shared_ptr<Room> RoomManager::GetRandomRoomAt(const RoomCoord& coord) {
	return std::make_shared<Room0>(env, coord);
}

bool RoomManager::CheckIfLive(const RoomCoord& coord) {
	// assume the given coord exist
	// check four nearby rooms, if one is focused, then this is alive
	auto ptr = GetRoomAt({ coord.x - 1, coord.y });
	if (ptr && ptr->isFocused) {
		return true;
	}
	ptr = GetRoomAt({ coord.x + 1, coord.y });
	if (ptr && ptr->isFocused) {
		return true;
	}
	ptr = GetRoomAt({ coord.x, coord.y - 1 });
	if (ptr && ptr->isFocused) {
		return true;
	}
	ptr = GetRoomAt({ coord.x, coord.y + 1 });
	if (ptr && ptr->isFocused) {
		return true;
	}
	// if no nearby is focused, this is dead
	return false;
}

RoomCoord RoomManager::Wolrd2RoomCoord(const Vector2& pos)
{
	return RoomCoord(
		(int)std::round(pos.x / (ROOM_WIDTH * TILE_LENGTH)),
		(int)std::round(pos.y / (ROOM_WIDTH * TILE_LENGTH))
		);
}
