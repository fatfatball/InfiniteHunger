#include "BaseItem.h"

BaseItem::BaseItem(GamesEngineeringBase::Window& env, const Vector2& pos) 
	: BasePhysicalObject(env) {
	globalPosition = pos;
	curState = { globalPosition, scale, rotation };
	prevState = curState;
	canCollide = true;
}

void BaseItem::OnStart() {
	sprite.load(GetSpritePath());
}

ColorRGBA BaseItem::GetColorAt(const float& u, const float& v) const {
	int x = std::clamp(u, 0.f, 1.f) * sprite.width;
	int y = std::clamp(v, 0.f, 1.f) * sprite.height;
	x = x == sprite.width ? x - 1 : x;
	y = y == sprite.height ? y - 1 : y;
	auto ptr = sprite.atUnchecked(x, y);
	ColorRGBA color;
	color.r = ptr[0];
	color.g = ptr[1];
	color.b = ptr[2];
	color.a = sprite.alphaAtUnchecked(x, y);
	return color;
}

inline unsigned char BaseItem::GetAlphaAt(const float& u, const float& v) const {
	int x = std::clamp(u, 0.f, 1.f) * sprite.width;
	int y = std::clamp(v, 0.f, 1.f) * sprite.height;
	x = x == sprite.width ? x - 1 : x;
	y = y == sprite.height ? y - 1 : y;
	return sprite.alphaAtUnchecked(x, y);
}
