#pragma once
#include "BaseGameObject.h"
#include<functional>

// simple timer object
class ObjectTimer : public BaseGameObject {
public:
	ObjectTimer(GamesEngineeringBase::Window& canvas) : BaseGameObject(canvas){
		time = 0.f;
		timeCounter = 0.f;
		isStarted = false;
	}

	void OnStart() override {}
	void FixedUpdate(const float& dt) override { TickTimer(dt); }
	void VisualUpdate(const float& dt) override {}
	void OnDestroy() override {}
	ColorRGBA GetColorAt(const float& u, const float& v) const override { return {}; }
	unsigned char GetAlphaAt(const float& u, const float& v) const override { return 0; }
	void SetTimer(const float& time, std::function<void()> callback) {
		this->time = time;
		this->timeCounter = time;
		this->callback = callback;
	}
	void StartTimer() {
		isStarted = true;
	}

private:
	float time;
	float timeCounter;
	bool isStarted;
	std::function<void()> callback;

	void TickTimer(const float& dt) {
		if (!isStarted) {
			return;
		}
		timeCounter -= dt;
		if (timeCounter <= 0) {
			callback();
			isPendingDestroy = true;
			isStarted = false;
		}
	}
};

