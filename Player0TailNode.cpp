#include "Player0TailNode.h"
#include "GamesEngineeringBase.h"

Player0TailNode::Player0TailNode(GamesEngineeringBase::Window& canvas) : BasePhysicalObject(canvas) {
	scale = Vector2(50, 50);
	globalPosition = { 0, 0 };
	rotation = PI / 4.f;
	maxVelocity = 400; // global position per second
	maxForce = 1000; // global position per second^2
	renderOrder = RenderOrder::Third;
	prevState = { globalPosition, scale, rotation };
	curState = { globalPosition, scale, rotation };
	isFixed = false;
	canCollide = true;
	collisionRadius = 7;
}

Player0TailNode::Player0TailNode(GamesEngineeringBase::Window& canvas, const Vector2& pos) : Player0TailNode(canvas) {
	globalPosition = pos;
	prevState = { globalPosition, scale, rotation };
	curState = { globalPosition, scale, rotation };
}

void Player0TailNode::OnStart() {
	sprite.load("./Resources/Characters/red_hand.png");
}

void Player0TailNode::FixedUpdate(const float& dt) {
	UpdatePhysicPosition(dt);
}

void Player0TailNode::VisualUpdate(const float& dt) {
}

void Player0TailNode::OnDestroy() {
}

ColorRGBA Player0TailNode::GetColorAt(const float& u, const float& v) const {
	int x = std::clamp(u, 0.f, 1.f) * sprite.width;
	int y = std::clamp(v, 0.f, 1.f) * sprite.height;
	auto ptr = sprite.atUnchecked(x, y);
	ColorRGBA color;
	color.r = ptr[0];
	color.g = ptr[1];
	color.b = ptr[2];
	color.a = sprite.alphaAtUnchecked(x, y);
	return color;
}

unsigned char Player0TailNode::GetAlphaAt(const float& u, const float& v) const {
	int x = std::clamp(u, 0.f, 1.f) * sprite.width;
	int y = std::clamp(v, 0.f, 1.f) * sprite.height;
	auto ptr = sprite.atUnchecked(x, y);
	return sprite.alphaAtUnchecked(x, y);
}