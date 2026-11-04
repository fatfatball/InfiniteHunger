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
#include "TextRenderer.h"
#include "ClickableText.h"

class SceneManager;
class BasePlayer;

// a scene contains lots of objects, and manage their life circles
class Scene{
protected:
	List<std::shared_ptr<BaseGameObject>> objects;
	List<std::shared_ptr<ClickableText>> uiObjects;
	GamesEngineeringBase::Window& canvas;
	Camera& camera;
	//List<RenderedItem> renderedItems;
	List<ScreenAABB> renderTiles;
	List<List<RenderedItem>> renderedItemLists{ CO_RENDER_THREADS + 1 };
	//List<std::shared_ptr<Tilemap>> tilemaps;
	std::shared_ptr<RoomManager> roomManager;
	List<std::shared_ptr<BasePlayer>> players;
	 
public:
	SceneManager* sceneManager;

	Scene(GamesEngineeringBase::Window& window, Camera& camera, SceneManager* manager);

	virtual ~Scene() {
		for (int i = 0; i < objects.GetSize(); ++i) {
			objects[i]->OnDestroy();
		}
	}

	void AddObject(std::shared_ptr<BaseGameObject> obj);
	void AddUIObject(std::shared_ptr<ClickableText> uiObject);
	void AddRoom(std::shared_ptr<Room> room);
	void RemoveRoom(const RoomCoord& coord);
	void AddPlayer(const PlayerType type);

	virtual void FixedUpdateObjects(const float& dt);
	virtual void VisualUpdateObjects(const float& dt);
	virtual void RenderObjects(const float& alpha);
	virtual void RecycleObjects();
	void RecycleAllObjects();

	void SetCameraTarget(BaseGameObject* target);
	Vector2 GetCameraTargetPos();
	void SetCameraBoundary(const AABB& newBound);
	AABB GetCameraAABB();
	BasePlayer* GetFirstPlayer();
	BasePlayer* GetClosestPlayer(const Vector2& pos); // find the closest player for enemy
	bool IsInRoom(Vector2& pos); // is a pos is inside of an instantiated room
	int GetObjectCount() const;
	RoomManager* GetRoomManager() { return roomManager.get(); }
	int GetPlayerCount() { return players.GetSize(); }
	PlayerSaveData GetPlayerSaveDataAt(const int& i);

private:
	void RenderWorker(const float& alpha, std::atomic<int>& nextTile, List<RenderedItem>& renderedItems);
};