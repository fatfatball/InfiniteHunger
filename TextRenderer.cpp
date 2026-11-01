#include "TextRenderer.h"
#include "Fonts.h"

void TextRenderer::DrawChar(
	GamesEngineeringBase::Window& env,
	const char& c, const int& x, const int& y, 
	const int& scale, const ColorRGBA& color) {
	if (c == ' ') {
		return;
	}

	int ind = c - 32;
	if (ind < 0 || ind >= 59) {
		return;
	}

	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 7; j++) {
			if ((FONT5x7[ind][i] >> j) & 1) {
				DrawRect(
					env, 
					x + i * scale, y + j * scale,
					scale, scale,
					color
				);
			}
		}
	}
}

void TextRenderer::DrawRect(
	GamesEngineeringBase::Window& env, const int& x, const int& y, const int& width, const int& height,
	const ColorRGBA& color) {
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			env.draw(x + i, y + j, color.r, color.g, color.b);
		}
	}
}

void TextRenderer::DrawText(
	GamesEngineeringBase::Window& env, const std::string & text, const int& x, const int& y,
	const int& scale, const ColorRGBA & color) {
	int cursorX = x;
	int cursorY = y;

	for (int i = 0; i < text.length(); i++) {
		if (text[i] == '\n') {
			cursorX = x;
			cursorY += 8 * scale;
			continue;
		}
		DrawChar(env, text[i], cursorX, cursorY, scale, color);
		cursorX += 6 * scale;
	}
}