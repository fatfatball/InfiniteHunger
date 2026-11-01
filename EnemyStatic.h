#pragma once
#include "BaseEnemy.h"
#include "GamesEngineeringBase.h"
#include "Vector2.h"
#include <string>
#include "List.h"
#include <memory>
#include "SmallStructs.h"

class EnemyStatic : public BaseEnemy {
public:
	int maxSpawnedEnemies;

	EnemyStatic(GamesEngineeringBase::Window& canvas, const Vector2& pos);

	void OnStart() override;
	void FixedUpdate(const float& dt) override;
	void VisualUpdate(const float& dt) override;
	ColorRGBA GetColorAt(const float& u, const float& v) const override;
	unsigned char GetAlphaAt(const float& u, const float& v) const override;

private:
	float spawnInverval;
	float spawnInvervalCounter;
	List<std::weak_ptr<BaseEnemy>> spawnedEnemies;

	std::string GetSpritePath() override;
	void SpawnEnemy();
	void TickSpawn(const float& dt);
};

