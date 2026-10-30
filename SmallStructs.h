#pragma once

class BaseGameObject;
class BasePhysicalObject;

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

	ColorRGBA(const int& r, const int& g, const int& b, const int& a) {
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}
};

struct RenderedItem {
	BaseGameObject* objPtr = nullptr;
	short depth = 0x7fff;
};

enum class ConstraintType {
	Distance,
	Angle,
	DirectionAngle,
};

struct Constraint {
	ConstraintType type;
	BasePhysicalObject* o1;
	BasePhysicalObject* o2;
	BasePhysicalObject* o3;
	float constraint1;
	float constraint2;
	float stiffness;
};

enum class RotationDefType {
	PrevAndNext,
	Prev,
};

struct RotationDefiner { // adjusts the node's rotation within a body line, always rotate o2
	RotationDefType type;
	BasePhysicalObject* o1; // if exist, prev or head
	BasePhysicalObject* o2; // to be rotated
	BasePhysicalObject* o3; // if exist, next
};

struct RoomCoord {
	int x = 0;
	int y = 0;
	bool operator==(const RoomCoord& other) const {
		return x == other.x && y == other.y;
	}
	int DistanceTo(const RoomCoord& other) const {
		return abs(x - other.x) + abs(y - other.y);
	}
};

enum class CharacterType {
	Player,
	Enemy,
	None,
};

enum class PlayerType {
	Swordsman,
	Archer,
	Wizard,
};

enum class EnemyType {
	Fast,
	Medium,
	Slow,
};