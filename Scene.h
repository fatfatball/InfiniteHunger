#pragma once
#include "BaseGameObject.h"
#include "GamesEngineeringBase.h"
#include "List.h"
#include "Camera.h"
#include <memory>
#include "SmallStructs.h"
#include <atomic>
#include "Room.h"
#include "RoomManager.h"

class BasePlayer;

// a scene contains lots of objects, and manage their life circles
class Scene{
private:
	List<std::shared_ptr<BaseGameObject>> objects;
	GamesEngineeringBase::Window& canvas;
	Camera& camera;
	//List<RenderedItem> renderedItems;
	List<ScreenAABB> renderTiles;
	List<List<RenderedItem>> renderedItemLists{ CO_RENDER_THREADS + 1 };
	//List<std::shared_ptr<Tilemap>> tilemaps;
	std::shared_ptr<RoomManager> roomManager;
	List< std::shared_ptr<BasePlayer>> players;
	 
public:
	Scene(GamesEngineeringBase::Window& window, Camera& camera);

	~Scene() {
		for (int i = 0; i < objects.GetSize(); ++i) {
			objects[i]->OnDestroy();
		}
	}

	void AddObject(std::shared_ptr<BaseGameObject> obj);
	void AddRoom(std::shared_ptr<Room> room);
	void RemoveRoom(const RoomCoord& coord);
	void AddPlayer(const PlayerType type);

	void FixedUpdateObjects(float dt);
	void VisualUpdateObjects(float dt);
	void RenderObjects(const float& alpha);
	void RecycleObjects();

	void SetCameraTarget(BaseGameObject* target);
	Vector2 GetCameraTargetPos();
	void SetCameraBoundary(const AABB& newBound);
	AABB GetCameraAABB();
	BasePlayer* GetFirstPlayer();
	BasePlayer* GetClosestPlayer(const Vector2& pos); // find the closest player for enemy
	bool IsInRoom(Vector2& pos); // is a pos is inside of an instantiated room
	int GetObjectCount() const;

private:
	void RenderWorker(const float& alpha, std::atomic<int>& nextTile, List<RenderedItem>& renderedItems);
};