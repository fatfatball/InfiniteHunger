#include "Scene.h"
#include "Tilemap.h"
#include <thread>
#include <functional>
#include "BasePhysicalObject.h"
#include "Vector2.h"
#include "Player0.h"
#include "BasePlayer.h"

Scene::Scene(GamesEngineeringBase::Window& window, Camera& camera) : canvas(window), camera(camera) {
	// slice the canvas into tiles, for rendering
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
	roomManager = std::make_shared<RoomManager>(canvas);
	roomManager->scene = this;
	AddObject(roomManager);
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

void Scene::AddRoom(std::shared_ptr<Room> room) {
	roomManager->AddRoom(room);
	AddObject(room);
}

void Scene::RemoveRoom(const RoomCoord& coord) {
	roomManager->RemoveRoom(coord);
}

void Scene::AddPlayer(const PlayerType type) {
	if (players.GetSize() >= 2) {
		return;
	}
	switch (type) {
	case PlayerType::Swordsman: {
		auto ptr = std::make_shared<Player0>(canvas);
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

void Scene::FixedUpdateObjects(float dt) {
	int size = objects.GetSize();
	for (int i = 0; i < size; ++i) {
		if (!objects[i]->isActive || objects[i]->isPendingDestroy) {
			continue;
		}
		objects[i]->FixedUpdate(dt);
	}

#pragma region CollisionSolving
	// solve collision here
	// 1. solve entity colliding entity.
	// 2. solve entity colliding tilemap.
	for (int i = 0; i < size; ++i) {
		for (int j = i + 1; j < size; ++j) {
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
			float wa = 1.f / a->mass; // please don't be 0
			float wb = 1.f / b->mass;
			float wSum = wa + wb; // and sum shouldn't be 0

			Vector2 normal = delta.Normalized();
			Vector2 position = a->globalPosition * 0.5f + b->globalPosition * 0.5f;
			a->globalPosition = a->globalPosition - normal * err * (wa / wSum);
			b->globalPosition = b->globalPosition + normal * err * (wb / wSum);

			a->OnCollision(b, normal * -1, position);
			b->OnCollision(a, normal, position);
		}
	}

	Vector2 normal;
	Vector2 position;
	for (int i = 0; i < roomManager->rooms.GetSize(); ++i) {
		for (int j = 0; j < size; ++j) {
			if (!objects[j]->canCollide) {
				continue;
			}
			auto ptr = dynamic_cast<BasePhysicalObject*>(objects[j].get());

			if (roomManager->rooms[i]->ResolveCollision(ptr, normal, position)) {
				ptr->OnCollision(roomManager->rooms[i].get(), normal, position);
			}
		}
	}

	// solve other constraints here
	for (int i = 0; i < size; ++i) {
		if (!objects[i]->isActive || objects[i]->isPendingDestroy) {
			continue;
		}
		objects[i]->SolveConstraints();
	}
#pragma endregion

	for (int i = 0; i < size; ++i) {
		if (!objects[i]->isActive || objects[i]->isPendingDestroy) {
			continue;
		}
		objects[i]->UpdateVelocity(dt);
	}

	for (int i = 0; i < size; ++i) {
		if (!objects[i]->isActive || objects[i]->isPendingDestroy) {
			continue;
		}
		objects[i]->UpdateStates();
	}
}

void Scene::VisualUpdateObjects(float dt) {
	int size = objects.GetSize();
	for (int i = 0; i < size; ++i) {
		if (!objects[i]->isActive || objects[i]->isPendingDestroy) {
			continue;
		}
		objects[i]->VisualUpdate(dt);
	}
}

void Scene::RenderObjects(const float& alpha) {
	int size = objects.GetSize();
	camera.ClearDepthGraph();

	for (int i = 0; i < size; ++i) {
		if (!objects[i]->isActive || objects[i]->isPendingDestroy) {
			continue;
		}
		objects[i]->UpdateRenderState(alpha, camera);
	}

	// render in order
	// first slice screen into tiles, then render each tile
	std::atomic<int> nextTile = 0;
	std::thread workers[CO_RENDER_THREADS];

	for (int i = 0; i < CO_RENDER_THREADS; ++i) {
		workers[i] = std::thread(&Scene::RenderWorker, this, std::ref(alpha), std::ref(nextTile), std::ref(renderedItemLists[i]));
	}
	RenderWorker(alpha, nextTile, renderedItemLists[CO_RENDER_THREADS]);
	for (int i = 0; i < CO_RENDER_THREADS; ++i) {
		workers[i].join();
	}
}

void Scene::RecycleObjects() {
	int size = objects.GetSize();
	for (int i = size - 1; i >= 0; --i) {
		if (!objects[i]->isPendingDestroy) {
			continue;
		}
		objects[i]->OnDestroy();
		objects.Erase(i);
	}
}

void Scene::SetCameraTarget(BaseGameObject* target) {
	camera.SetFollowTarget(target);
}

Vector2 Scene::GetCameraTargetPos() {
	if (camera.followTarget) {
		return camera.followTarget->globalPosition;
	}
	else {
		return Vector2();
	}
}

void Scene::SetCameraBoundary(const AABB& newBound) {
	camera.SetBoundary(newBound);
}

AABB Scene::GetCameraAABB() {
	return camera.GetAABB();
}

BasePlayer* Scene::GetFirstPlayer() {
	if (players.GetSize() >= 1) {
		return players[0].get();
	}
	return nullptr;
}

BasePlayer* Scene::GetClosestPlayer(const Vector2& pos) {
	if (players.GetSize() == 0) {
		return nullptr;
	}
	BasePlayer* res = players[0].get();
	float dist = (res->globalPosition - pos).Length();
	for (int i = 1; i < players.GetSize(); ++i) {
		if ((players[i]->globalPosition - pos).Length() < dist) {
			res = players[i].get();
		}
	}
	return res;
}

bool Scene::IsInRoom(Vector2& pos) {
	RoomCoord coord = roomManager->Wolrd2RoomCoord(pos);
	return roomManager->HasRoom(coord);
}

void Scene::RenderWorker(const float& alpha, std::atomic<int>& nextTile, List<RenderedItem>& renderedItems) {
	int tile;
	while ((tile = nextTile.fetch_add(1, std::memory_order_relaxed)) < renderTiles.GetSize()) {
		bool isRendered;
		short depth = 0;
		// depth test
		for (int order = 2; order >= 0; --order) {
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
