#pragma once
#include "Vector2.h"
#include <string>

class BaseGameObject;
class BasePhysicalObject;

enum RenderOrder {
	First,
	Second,
	Third,
	HUD,
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

enum class BodyNodeType {
	Normal,
	Heal,
};

struct RunStat {
	int playerCount = 0;
	int totalEnemyKilled = 0;
	double totalTime = 0.0;
};

struct RoomSave {
	RoomCoord coord;
	bool completed[4] = { false, false, false, false };

	std::string ToString() {
		std::string res;
		res = std::to_string(coord.x) + " " + std::to_string(coord.y) + "\n";
		res += std::to_string(completed[0]) + " "
			+ std::to_string(completed[1]) + " "
			+ std::to_string(completed[2]) + " "
			+ std::to_string(completed[3]) + "\n";
		return res;
	}
};

struct PlayerSaveData {
	bool isAlive = false;
	PlayerType type = PlayerType::Swordsman;
	Vector2 pos = {};
	int curHealth = 0;
	float selfDamageTime = 0.f;
	float selfDamageCounter = 0.f;
	int keyCount = 0;

	std::string ToString() const {
		std::string res;

		res = std::to_string(isAlive) + "\n";
		switch (type) {
		case PlayerType::Swordsman:
			res += "Swordsman\n";
			break;
		case PlayerType::Archer:
			res += "Archer\n";
			break;
		case PlayerType::Wizard:
			res += "Wizard\n";
			break;
		default:
			res += "Swordsman\n";
			break;
		}
		res += std::to_string(pos.x) + " " + std::to_string(pos.y) + "\n";
		res += std::to_string(curHealth) + "\n";
		res += std::to_string(selfDamageTime) + "\n";
		res += std::to_string(selfDamageCounter) + "\n";
		res += std::to_string(keyCount) + "\n";

		return res;
	}
};

struct SaveContext {
	int roomSeed = 0;
	RoomCoord startRoom = {};
	RunStat runStat;

	int playerCount =  1;
	PlayerSaveData playerSave[2];

	std::string ToString() const {
		std::string res;
		res = std::to_string(roomSeed) + "\n";
		res += std::to_string(startRoom.x) + " " + std::to_string(startRoom.y) + "\n";
		res += std::to_string(runStat.playerCount) + " " + std::to_string(runStat.totalEnemyKilled)
			+ " " + std::to_string(runStat.totalTime) + "\n";
		res += std::to_string(playerCount) + "\n";
		res += playerSave[0].ToString();
		res += playerSave[1].ToString();
		return res;
	}
};