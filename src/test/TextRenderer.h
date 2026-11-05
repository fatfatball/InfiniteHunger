#pragma once
#include "GamesEngineeringBase.h"
#include "AABB.h"
#include <string>
#include "SmallStructs.h"

// render a text onto the screen
class TextRenderer {
private:
	static void DrawChar(GamesEngineeringBase::Window& env, const char& c, const int& x, const int& y, const int& scale, const ColorRGBA& color);
	static void DrawRect(GamesEngineeringBase::Window& env, const int& x, const int& y, const int& width, const int& height, const ColorRGBA& color);

public:
	static void DrawText(GamesEngineeringBase::Window& env, const std::string& text, const int& x, const int& y, const int& scale, const ColorRGBA& color);
	static ScreenAABB GetTextScreenAABB(const std::string& text, const int& x, const int& y, const int& scale);
};

