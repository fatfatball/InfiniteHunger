#pragma once

class BaseGameObject;

enum RenderOrder {
	First,
	Second,
	Third,
};

struct ColorRGBA {
	unsigned char r = 0;
	unsigned char g = 0;
	unsigned char b = 0;
	unsigned char a = 0;

	ColorRGBA() {
		r = 0;
		g = 0;
		b = 0;
		a = 0;
	}
};

struct RenderedItem {
	BaseGameObject* objPtr = nullptr;
	short depth = 0x7fff;
};