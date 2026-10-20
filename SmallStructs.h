#pragma once

enum RenderOrder {
	First,
	Second,
	Third,
};

class ColorRGBA {
public:
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