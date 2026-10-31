#include "Player2.h"
#include "PlayerTailNode.h"
#include <cmath>
#include "Vector2.h"
#include "PlayerAttack2.h"
#include "PlayerSkill2.h"
#include <memory>

Player2::Player2(GamesEngineeringBase::Window& canvas, const bool& isFirst) : BasePlayer(canvas, isFirst) {
	skill = std::make_unique<PlayerSkill2>(this);
}

void Player2::OnStart() {
	BasePlayer::OnStart();
}

void Player2::FixedUpdate(const float& dt) {
	BasePlayer::FixedUpdate(dt);
}

bool Player2::Attack() {
	if (attackCooldownCounter > 0) {
		return false;
	}

	auto ptr = std::make_shared<PlayerAttack2>(env, this, globalPosition);
	scene->AddObject(ptr);

	attackCooldownCounter = attackCooldown;
	return true;
}

std::string Player2::GetSpritePath() {
	return "./Resources/Characters/purple_character.png";
}

std::shared_ptr<BaseBodyNode> Player2::GetBodyNode(GamesEngineeringBase::Window& env, const Vector2& pos) {
	return std::make_shared<PlayerTailNode>(env, pos, this, CharacterType::Player, BodyNodeType::Normal, PlayerType::Wizard);
}
