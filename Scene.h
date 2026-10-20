#pragma once
#include "BaseGameObject.h"
#include "GamesEngineeringBase.h"
#include "List.h"
#include "Camera.h"
#include <memory>

// a scene contains lots of objects, and manage their life circles
class Scene{
private:
	List<std::shared_ptr<BaseGameObject>> objects;
	GamesEngineeringBase::Window& canvas;
	Camera& camera;

public:
	Scene(GamesEngineeringBase::Window& window, Camera& camera) : canvas(window), camera(camera) {}

	~Scene() {
		for (int i = 0; i < objects.GetSize(); ++i) {
			objects[i]->OnDestroy();
		}
	}

	void AddObject(std::shared_ptr<BaseGameObject>&& obj);
	void FixedUpdateObjects(float dt);
	void VisualUpdateObjects(float dt);
	void RenderObjects(const float& alpha);
	void RecycleObjects();
};