#include "Player1.h"
#include "PlayerTailNode.h"
#include "PlayerAttack1.h"
#include <cmath>
#include "Vector2.h"

Player1::Player1(GamesEngineeringBase::Window& canvas) : BasePlayer(canvas) {

}

void Player1::OnStart() {
	BasePlayer::OnStart();
}

void Player1::FixedUpdate(const float& dt) {
	BasePlayer::FixedUpdate(dt);
}

bool Player1::Attack() {
	if (attackCooldownCounter > 0) {
		return false;
	}

	auto ptr = std::make_shared<PlayerAttack1>(env, this, globalPosition);
	scene->AddObject(ptr);

	attackCooldownCounter = attackCooldown;
	return true;
}

std::string Player1::GetSpritePath() {
	return "./Resources/Characters/yellow_character.png";
}

std::shared_ptr<BaseBodyNode> Player1::GetBodyNode(GamesEngineeringBase::Window& env, const Vector2& pos) {
	return std::make_shared<PlayerTailNode>(env, pos, this, CharacterType::Player, BodyNodeType::Normal, PlayerType::Archer);
}
