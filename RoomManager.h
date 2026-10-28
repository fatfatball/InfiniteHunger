#pragma once
#include "BaseGameObject.h"
#include "GamesEngineeringBase.h"
#include "Room.h"
#include <memory>
#include "SmallStructs.h"
#include "Vector2.h"

class RoomManager : public BaseGameObject {
public:
	List<std::shared_ptr<Room>> rooms;
	RoomCoord prevRoom;

	RoomManager(GamesEngineeringBase::Window& canvas);

	void OnStart() override;
	void FixedUpdate(const float& dt) override;
	void VisualUpdate(const float& dt) override;
	void OnDestroy() override;
	ColorRGBA GetColorAt(const float& u, const float& v) const override;
	unsigned char GetAlphaAt(const float& u, const float& v) const override;

	void AddRoom(std::shared_ptr<Room> room);
	void RemoveRoom(const RoomCoord coord);
	bool HasRoom(const RoomCoord& coord);
	Room* GetRoomAt(const RoomCoord& coord);
	std::shared_ptr<Room> GetRandomRoomAt(const RoomCoord& coord);
	bool CheckIfLive(const RoomCoord& coord);
	RoomCoord Wolrd2RoomCoord(const Vector2& pos);
};