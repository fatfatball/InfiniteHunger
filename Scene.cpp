#include "Scene.h"

void Scene::AddObject(std::shared_ptr<BaseGameObject>&& obj) {
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
	renderedItems.Clear();
	renderedItems.Reserve(size);
	camera.ClearDepthGraph();
	short depth = 0;
	// depth test
	for (int order = 2; order >= 0; --order) {
		for (int i = size - 1; i >= 0; --i) {
			if (!objects[i]->isActive || objects[i]->isPendingDestroy) {
				continue;
			}
			if (objects[i]->renderOrder == order) {
				objects[i]->UpdateRenderState(alpha);
				objects[i]->DepthTest(alpha, camera, depth);
				renderedItems.PushBack({ objects[i].get() , depth });
				++depth;
			}
		}
	}

	 // render in order
	 // first slice screen into tiles, then render each tile
	ScreenAABB renderTile = { 0, 0, 0, 0 };
	for (short yPos = 0; yPos <= WINDOW_HEIGHT; yPos += RENDER_TILE_HEIGHT) {
		for (short xPos = 0; xPos <= WINDOW_WIDTH; xPos += RENDER_TILE_WIDTH) {
			renderTile.minX = xPos;
			renderTile.maxX = std::min(xPos + RENDER_TILE_WIDTH - 1, WINDOW_WIDTH - 1);
			renderTile.minY = yPos;
			renderTile.maxY = std::min(yPos + RENDER_TILE_HEIGHT - 1, WINDOW_HEIGHT - 1);
			for (int i = 0; i < renderedItems.GetSize(); ++i) {
				renderedItems[i].objPtr->RenderSelf(alpha, camera, renderedItems[i].depth, renderTile);
			}
		}
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