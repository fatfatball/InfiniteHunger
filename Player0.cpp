#include "Player0.h"
#include "Scene.h"
#include "PolyLine2D.h"
#include "Vector2.h"
#include <cmath>
#include "SmallStructs.h"
#include "BaseBodyNode.h"
#include "BasePlayer.h"
#include "PlayerTailNode.h"
#include "PlayerAttack0.h"
#include "GamesEngineeringBase.h"
#include "PlayerSkill0.h"
#include <memory>

Player0::Player0(GamesEngineeringBase::Window& canvas, const bool& isFirst) : BasePlayer(canvas, isFirst){
	skill = std::make_unique<PlayerSkill0>(this);
}

void Player0::OnStart() {
	BasePlayer::OnStart();
}

void Player0::FixedUpdate(const float& dt) {
	BasePlayer::FixedUpdate(dt);
}

bool Player0::Attack()
{
	if (attackCooldownCounter > 0) {
		return false;
	}

	auto ptr = std::make_shared<PlayerAttack0>(env, this, globalPosition);
	scene->AddObject(ptr);

	attackCooldownCounter = attackCooldown;
	return true;
}

std::string Player0::GetSpritePath()
{
	return "./Resources/Characters/red_character.png";
}

std::shared_ptr<BaseBodyNode> Player0::GetBodyNode(
	GamesEngineeringBase::Window& env, const Vector2& pos) {
	return std::make_shared<PlayerTailNode>(env, pos, this, CharacterType::Player, BodyNodeType::Normal, PlayerType::Swordsman);
}