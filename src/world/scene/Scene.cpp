#include "Scene.h"
#include "Tilemap.h"
#include <thread>
#include <functional>
#include "BasePhysicalObject.h"
#include "Vector2.h"
#include "Player0.h"
#include "BasePlayer.h"
#include "Player1.h"
#include "Player2.h"

Scene::Scene(GamesEngineeringBase::Window& window, Camera& camera, SceneManager* manager) 
	: canvas(window), camera(camera), sceneManager(manager) {
	// slice the canvas into tiles for multi-thread rendering
	ScreenAABB renderTile = { 0, 0, 0, 0 };
	for (short yPos = 0; yPos < WINDOW_HEIGHT; yPos += RENDER_TILE_HEIGHT) {
		for (short xPos = 0; xPos < WINDOW_WIDTH; xPos += RENDER_TILE_WIDTH) {
			renderTile.minX = xPos;
			renderTile.maxX = std::min(xPos + RENDER_TILE_WIDTH - 1, WINDOW_WIDTH - 1);
			renderTile.minY = yPos;
			renderTile.maxY = std::min(yPos + RENDER_TILE_HEIGHT - 1, WINDOW_HEIGHT - 1);
			renderTiles.PushBack(renderTile);
		}
	}
}

void Scene::AddObject(std::shared_ptr<BaseGameObject> obj) {
	// no checking if already exists
	if (!obj) {
		return;
	}
	BaseGameObject* raw = obj.get();
	raw->scene = this;
	objects.PushBack(obj);
	raw->OnStart();
}

void Scene::AddUIObject(std::shared_ptr<ClickableText> uiObject) {
	if (!uiObject) {
		return;
	}
	ClickableText* raw = uiObject.get();
	uiObjects.PushBack(uiObject);
	raw->OnStart();
}

void Scene::AddRoom(std::shared_ptr<Room> room) {
	if (roomManager->AddRoom(room)) {
		AddObject(room);
	}
}

void Scene::RemoveRoom(const RoomCoord& coord) {
	roomManager->RemoveRoom(coord);
}

void Scene::AddPlayer(const PlayerType type) {
	if (players.GetSize() >= 2) {
		return;
	}
	bool isFirstPlayer = !(players.GetSize() > 0);
	switch (type) {
	case PlayerType::Swordsman: {
		auto ptr = std::make_shared<Player0>(canvas, isFirstPlayer);
		ptr->TeleportTo(camera.globalPosition);
		AddObject(ptr);
		players.PushBack(ptr);
		break;
	}
	case PlayerType::Archer: {
		auto ptr = std::make_shared<Player1>(canvas, isFirstPlayer);
		ptr->TeleportTo(camera.globalPosition);
		AddObject(ptr);
		players.PushBack(ptr);
		break;
	}
	case PlayerType::Wizard: {
		auto ptr = std::make_shared<Player2>(canvas, isFirstPlayer);
		ptr->TeleportTo(camera.globalPosition);
		AddObject(ptr);
		players.PushBack(ptr);
		break;
	}
	default:
		break;
	}
	if (players.GetSize() == 1) {
		SetCameraTarget(players[0].get());
	}
}

void Scene::AddPlayer(const PlayerType& type, const Vector2& pos,
	const float& curHealth, const float& selfDamageTime, const float& selfDamageCounter, const int& keyCount) {
	if (players.GetSize() >= 2) {
		return;
	}
	bool isFirstPlayer = !(players.GetSize() > 0);
	std::shared_ptr<BasePlayer> ptr;

	switch (type) {
	case PlayerType::Swordsman: {
		ptr = std::make_shared<Player0>(canvas, isFirstPlayer);
		break;
	}
	case PlayerType::Archer: {
		ptr = std::make_shared<Player1>(canvas, isFirstPlayer);
		break;
	}
	case PlayerType::Wizard: {
		ptr = std::make_shared<Player2>(canvas, isFirstPlayer);
		break;
	}
	default:
		ptr = std::make_shared<Player0>(canvas, isFirstPlayer);
		break;
	}

	ptr->TeleportTo(pos);
	ptr->SetCurHealth(curHealth);
	ptr->SetKeyCount(keyCount);
	ptr->selfDamageTime = selfDamageTime;
	ptr->selfDamageTimeCounter = selfDamageCounter;
	if (curHealth < 1e-4f) {
		ptr->isDead = true;
	}
	AddObject(ptr);
	players.PushBack(ptr);

	if (!ptr->isDead) {
		SetCameraTarget(players[players.GetSize() - 1].get());
	}
}

void Scene::FixedUpdateObjects(const float& dt) {
	// Position Based Dynamics, https://doi.org/10.1016/j.jvcir.2007.01.005
	// let a be acceleration, v be velocity, x be position, t be time, dt be delta time.
	// 1. get a(t+dt) from input
	// 2. v(t) + a(t+dt)dt -> v(t+dt)
	// 3. damp velocity: v(t+dt) * dampPercentPerSecond^dt -> v(t+dt)
	// 4. x(t) + v(t+dt)dt -> p(t+dt)	(p for predicted x because it'll be adjusted afterwards)
	int size = objects.GetSize();
	for (int i = 0; i < size; ++i) {
		if (!objects[i]->isActive || objects[i]->isPendingDestroy) {
			continue;
		}
		objects[i]->FixedUpdate(dt);
	}

	// 5. solve for physical constrains and collisions (here only 1 iteration):
	// p(t+dt) + constraintProjection -> p'(t+dt)
#pragma region ConstraintSolving
	// -1. solve entity colliding entity.
	for (int i = 0; i < size; ++i) {
		if (!objects[i]->isActive || objects[i]->isPendingDestroy) {
			continue;
		}
		for (int j = i + 1; j < size; ++j) {
			if (!objects[j]->isActive || objects[j]->isPendingDestroy) {
				continue;
			}
			auto o1 = objects[i].get();
			auto o2 = objects[j].get();
			if (!o1->canCollide || !o2->canCollide) {
				continue;
			}
			auto a = dynamic_cast<BasePhysicalObject*>(o1);
			auto b = dynamic_cast<BasePhysicalObject*>(o2);
			// maybe just let me know if there's an error
			//if (!a || !b) {
			//	continue;
			//}

			Vector2 delta = b->globalPosition - a->globalPosition;
			float dist = delta.Length();
			float minDist = b->collisionRadius + a->collisionRadius;

			if (dist >= minDist || dist < 1e-4f) { // if too close, that's an error, just wait until they break up
				continue;
			}

			float err = minDist - dist;
			Vector2 normal = delta.Normalized();
			Vector2 position = a->globalPosition * 0.5f + b->globalPosition * 0.5f;
			
			// apply collision constraint: move the position
			if (!a->isTrigger && !b->isTrigger) {
				float wa;
				float wb;
				if (a->isStatic) {
					wa = 0.f;
				}
				else {
					wa = 1.f / a->mass;
				}
				if (b->isStatic) {
					wb = 0.f;
				}
				else {
					wb = 1.f / b->mass;
				}
				float wSum = wa + wb;

				if (wSum > 1e-4f) {
					a->globalPosition = a->globalPosition - normal * err * (wa / wSum);
					b->globalPosition = b->globalPosition + normal * err * (wb / wSum);
				}
			}

			a->OnCollision(b, normal * -1, position);
			b->OnCollision(a, normal, position);
		}
	}

	// -2. solve entity colliding tilemap.
 	if (roomManager && roomManager->rooms.GetSize() != 0) {
		Vector2 normal;
		Vector2 position;
		for (int i = 0; i < roomManager->rooms.GetSize(); ++i) {
			for (int j = 0; j < size; ++j) {
				if (!objects[j]->canCollide || !objects[j]->isActive || objects[j]->isPendingDestroy) {
					continue;
				}
				auto ptr = dynamic_cast<BasePhysicalObject*>(objects[j].get());

				if (roomManager->rooms[i]->ResolveCollision(ptr, normal, position)) {
					ptr->OnCollision(roomManager->rooms[i].get(), normal, position);
				}
			}
		}
	}
	
	// -3. solve other constraints here (distance, angle, ...)
	for (int i = 0; i < size; ++i) {
		if (!objects[i]->isActive || objects[i]->isPendingDestroy) {
			continue;
		}
		objects[i]->SolveConstraints();
	}
#pragma endregion

	// 6. update velocity: [p'(t+dt) - x(t)] / dt -> v(t+dt)
	// notice that this is an approximation, using v(t+dt/2) as v(t+dt)
	//	so it has an error of O(1)
	// a better solution called Velocity Verlet Integration is: 
	//	[a(t+dt) + a(t)] / 2 * dt + v(t) -> v(t+dt)
	// but it's not compatible with constraints projection, 
	//	since the projection uses an implicit force to move objects.
	for (int i = 0; i < size; ++i) {
		if (!objects[i]->isActive || objects[i]->isPendingDestroy) {
			continue;
		}
		objects[i]->UpdateVelocity(dt);
	}

	// 7. apply new pos: p'(t+dt) -> x(t+dt)
	for (int i = 0; i < size; ++i) {
		if (!objects[i]->isActive || objects[i]->isPendingDestroy) {
			continue;
		}
		objects[i]->UpdateStates();
	}
}

void Scene::VisualUpdateObjects(const float& dt) {
	int size = objects.GetSize();
	for (int i = 0; i < size; ++i) {
		if (!objects[i]->isActive || objects[i]->isPendingDestroy) {
			continue;
		}
		objects[i]->VisualUpdate(dt);
	}

	size = uiObjects.GetSize();
	for (int i = 0; i < size; ++i) {
		if (!uiObjects[i] || uiObjects[i]->isPendingDestroy) {
			continue;
		}
		uiObjects[i]->VisualUpdate(dt);
	}
}

void Scene::RenderObjects(const float& alpha) {
	int size = objects.GetSize();
	camera.ClearDepthGraph();

	// 1. interpolate the renderState by prevState and curState, 
	//		because the visual update is not consistent with fixed update.
	for (int i = 0; i < size; ++i) {
		if (!objects[i]->isActive || objects[i]->isPendingDestroy) {
			continue;
		}
		objects[i]->UpdateRenderState(alpha, camera);
	}

	// 2. build threads to render
	std::atomic<int> nextTile = 0;
	std::thread workers[CO_RENDER_THREADS];

	for (int i = 0; i < CO_RENDER_THREADS; ++i) {
		workers[i] = std::thread(&Scene::RenderWorker, this, std::ref(alpha), std::ref(nextTile), std::ref(renderedItemLists[i]));
	}
	RenderWorker(alpha, nextTile, renderedItemLists[CO_RENDER_THREADS]);
	for (int i = 0; i < CO_RENDER_THREADS; ++i) {
		workers[i].join();
	}

	// 3. render UI objects at last
	size = uiObjects.GetSize();
	for (int i = 0; i < size; ++i) {
		if (!uiObjects[i] || uiObjects[i]->isPendingDestroy) {
			continue;
		}
		uiObjects[i]->RenderSelf();
	}
}

void Scene::RecycleObjects() {
	int size = objects.GetSize();
	for (int i = size - 1; i >= 0; --i) {
		if (!objects[i]->isPendingDestroy) {
			continue;
		}
		// clear other places
		auto ptr = objects[i].get();
		if (camera.followTarget == ptr) {
			camera.followTarget = nullptr;
		}
		// clear objects
		auto playerPtr = dynamic_cast<BasePlayer*>(ptr);
		if (playerPtr) {
			for (int j = 0; j < players.GetSize(); ++j) {
				if (players[j].get() == playerPtr) {
					players.Erase(j);
					break;
				}
			}
		}

		objects[i]->OnDestroy();
		objects.Erase(i);
	}

	// recycle UI objects
	size = uiObjects.GetSize();
	for (int i = size - 1; i >= 0; --i) {
		if (!uiObjects[i] || !uiObjects[i]->isPendingDestroy) {
			continue;
		}
		uiObjects[i]->OnDestroy();
		uiObjects.Erase(i);
	}
}

void Scene::RecycleAllObjects() {
	// recycle the objects to trigger their OnDestroy function

	int size = objects.GetSize();
	for (int i = size - 1; i >= 0; --i) {
		objects[i]->OnDestroy();
		objects.Erase(i);
	}

	size = uiObjects.GetSize();
	for (int i = size - 1; i >= 0; --i) {
		uiObjects[i]->OnDestroy();
		uiObjects.Erase(i);
	}
}

void Scene::SetCameraTarget(BaseGameObject* target) {
	camera.SetFollowTarget(target);
}

BaseGameObject* Scene::GetCameraTarget() {
	return camera.followTarget;
}

Vector2 Scene::GetCameraTargetPos() {
	if (camera.followTarget) {
		return camera.followTarget->globalPosition;
	}
	else {
		return camera.globalPosition;
	}
}

void Scene::SetCameraBoundary(const AABB& newBound) {
	camera.SetBoundary(newBound);
}

AABB Scene::GetCameraAABB() {
	return camera.GetAABB();
}

BasePlayer* Scene::GetFirstPlayer() {
	for (int i = 0; i < players.GetSize(); ++i) {
		if (players[i] && players[i]->isActive && !players[i]->isPendingDestroy && !players[i]->isDead) {
			return players[i].get();
		}
	}
	return nullptr;
}

BasePlayer* Scene::GetClosestPlayer(const Vector2& pos) {
	if (players.GetSize() == 0) {
		return nullptr;
	}
	BasePlayer* res = nullptr;
	float dist = 0.f;
	for (int i = 0; i < players.GetSize(); ++i) {
		if (players[i] && players[i]->isActive && !players[i]->isPendingDestroy && !players[i]->isDead) {
			float d = (players[i]->globalPosition - pos).Length();
			if (res == nullptr || d < dist) {
				dist = d;
				res = players[i].get();
			}
		}
	}
	return res;
}

bool Scene::IsInRoom(Vector2& pos) {
	RoomCoord coord = roomManager->Wolrd2RoomCoord(pos);
	return roomManager->HasRoom(coord);
}

int Scene::GetObjectCount() const
{
	return objects.GetSize();
}

PlayerSaveData Scene::GetPlayerSaveDataAt(const int& i) {
	PlayerSaveData data;
	if (i > players.GetSize() - 1) {
		return data;
	}

	data.isAlive = !players[i]->isDead;
	data.type = players[i]->playerType;
	data.pos = players[i]->globalPosition;
	data.curHealth = players[i]->GetCurHealth();
	data.selfDamageTime = players[i]->selfDamageTime;
	data.selfDamageCounter = players[i]->selfDamageTimeCounter;
	data.keyCount = players[i]->GetKeyCount();
	return data;
}

void Scene::RenderWorker(const float& alpha, std::atomic<int>& nextTile, List<RenderedItem>& renderedItems) {
	int tile;
	while ((tile = nextTile.fetch_add(1, std::memory_order_relaxed)) < renderTiles.GetSize()) {
		bool isRendered;
		short depth = 0;
		// depth test
		for (int order = 3; order >= 0; --order) {
			for (int i = objects.GetSize() - 1; i >= 0; --i) {
				if (!objects[i]->isActive || objects[i]->isPendingDestroy) {
					continue;
				}
				if (objects[i]->renderOrder == order) {
					isRendered = objects[i]->DepthTest(alpha, camera, depth, renderTiles[tile]);
					if (isRendered) {
						renderedItems.PushBack({ objects[i].get() , depth });
						++depth;
					}
				}
			}
		}

		for (int i = renderedItems.GetSize() - 1; i >= 0; --i) {
			renderedItems[i].objPtr->RenderSelf(alpha, camera, renderedItems[i].depth, renderTiles[tile]);
		}

		renderedItems.Clear();
	}
}

void Scene::RenderWorkerNoDepth(const float& alpha, std::atomic<int>& nextTile,
	List<RenderedItem>& renderedItems) {
	int tile;
	while ((tile = nextTile.fetch_add(1, std::memory_order_relaxed)) < renderTiles.GetSize()) {
		for (int order = 0; order <= 3; ++order) {
			for (int i = 0; i < objects.GetSize(); ++i) {
				if (!objects[i]->isActive || objects[i]->isPendingDestroy) {
					continue;
				}
				if (objects[i]->renderOrder == order) {
					objects[i]->RenderSelf(alpha, camera, -1, renderTiles[tile]);
				}
			}
		}
	}
}