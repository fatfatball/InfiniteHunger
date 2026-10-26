#include "Scene.h"
#include <thread>
#include <functional>

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
}

void Scene::AddObject(std::shared_ptr<BaseGameObject> obj) {
	// no checking if already exists
	if (!obj) {
		return;
	}
	BaseGameObject* raw = obj.get();
	raw->scene = this;
	objects.PushBack(std::move(obj));
	raw->OnStart();
}

void Scene::FixedUpdateObjects(float dt) {
	int size = objects.GetSize();
	for (int i = 0; i < size; ++i) {
		if (!objects[i]->isActive || objects[i]->isPendingDestroy) {
			continue;
		}
		objects[i]->FixedUpdate(dt);
	}

	for (int i = 0; i < size; ++i) {
		if (!objects[i]->isActive || objects[i]->isPendingDestroy) {
			continue;
		}
		objects[i]->SolveConstraints();
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
