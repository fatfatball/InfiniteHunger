#pragma once
#include "GamesEngineeringBase.h"
#include <string>
#include "Vector2.h"
#include <functional>
#include "AABB.h"
#include "SmallStructs.h"

// text object, to render directly on the screen, but not in world axis.
class ClickableText{
private:
	std::string text;
	GamesEngineeringBase::Window& env;
	Vector2 position;
	int scale;
	std::function<void()> OnClick;
	ColorRGBA color;
	ColorRGBA originColor;
	ScreenAABB screenAABB;
	bool prevMousePressed = false;

public:
	bool isPendingDestroy;

	ClickableText(GamesEngineeringBase::Window& canvas, const std::string& text,
		const Vector2& position, const int& scale, const ColorRGBA& color = { 200, 200, 200, 255 });

	ClickableText(GamesEngineeringBase::Window& canvas, const std::string& text,
		const Vector2& position, const int& scale, std::function<void()> OnClick, const ColorRGBA& color = { 200, 200, 200, 255 });
	
	void OnStart();
	void VisualUpdate(const float& dt);
	void OnDestroy();
	void RenderSelf();
	void SetText(const std::string& newText);
};
