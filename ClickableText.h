#pragma once
#include "GamesEngineeringBase.h"
#include <string>
#include "Vector2.h"
#include <functional>
#include "AABB.h"
#include "SmallStructs.h"

class ClickableText{
private:
	std::string text;
	GamesEngineeringBase::Window& env;
	Vector2 position;
	int scale;
	std::function<void()> OnClick;
	ColorRGBA color;
	ScreenAABB screenAABB;
	bool prevMousePressed = false;

public:
	bool isPendingDestroy;

	ClickableText(GamesEngineeringBase::Window& canvas, const std::string& text,
		const Vector2& position, const int& scale);

	ClickableText(GamesEngineeringBase::Window& canvas, const std::string& text,
		const Vector2& position, const int& scale, std::function<void()> OnClick);
	
	void OnStart();
	void VisualUpdate(const float& dt);
	void OnDestroy();
	void RenderSelf();
	void SetText(const std::string& newText);
};
