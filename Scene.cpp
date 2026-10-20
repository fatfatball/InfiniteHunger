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
	List<std::shared_ptr<BaseGameObject>> firstOrder;
	List<std::shared_ptr<BaseGameObject>> secondOrder;
	List<std::shared_ptr<BaseGameObject>> thirdOrder;
	
	for (int i = 0; i < size; ++i) {
		if (!objects[i]->isActive || objects[i]->isPendingDestroy) {
			continue;
		}

		switch (objects[i]->renderOrder) {
		case RenderOrder::First:
			firstOrder.PushBack(objects[i]);
			break;
		case RenderOrder::Second:
			secondOrder.PushBack(objects[i]);
			break;
		case RenderOrder::Third:
			thirdOrder.PushBack(objects[i]);
			break;
		default:
			break;
		}
	}

	for (int i = 0; i < firstOrder.GetSize(); ++i) {
		firstOrder[i]->RenderSelf(alpha, camera);
	}
	for (int i = 0; i < secondOrder.GetSize(); ++i) {
		secondOrder[i]->RenderSelf(alpha, camera);
	}
	for (int i = 0; i < thirdOrder.GetSize(); ++i) {
		thirdOrder[i]->RenderSelf(alpha, camera);
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

