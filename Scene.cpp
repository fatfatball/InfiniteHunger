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
	for (int order = 0; order < 3; ++order) {
		for (int i = 0; i < size; ++i) {
			if (!objects[i]->isActive || objects[i]->isPendingDestroy) {
				continue;
			}
			if (objects[i]->renderOrder == order) {
				objects[i]->RenderSelf(alpha, camera);
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

