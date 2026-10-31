#include "Player0.h"
#include "Scene.h"
#include "PolyLine2D.h"
#include "Vector2.h"
#include <cmath>
#include "SmallStructs.h"
#include "BaseBodyNode.h"
#include "BasePlayer.h"
#include "Player0TailNode.h"

Player0::Player0(GamesEngineeringBase::Window& canvas) : BasePlayer(canvas){

}

void Player0::OnStart() {
	BasePlayer::OnStart();
}

void Player0::FixedUpdate(const float& dt) {
	if (!isDead) {
		Vector2 inputDir = Vector2();
		if (env.keyPressed(0x57)) { // W
			inputDir.y += -1;
		}
		if (env.keyPressed(0x41)) { // A
			inputDir.x += -1;
		}
		if (env.keyPressed(0x53)) { // S
			inputDir.y += 1;
		}
		if (env.keyPressed(0x44)) { // D
			inputDir.x += 1;
		}

		// for test 
		if (env.keyPressed(0x20)) { // space
			Add1BodyNode();
		}

		// the force calculation algorithm is still naive
		force = Vector2(std::cos(rotation), std::sin(rotation)) * maxForce * -inputDir.y;
		rotation = Math::WrapAngle(rotation + inputDir.x * rotationSpeed * dt);
	}
	BaseCharacter::FixedUpdate(dt);
}

bool Player0::TakeDamage(const float& damage) {
	if (isDead || isInvulnerable || isPendingDestroy) {
		return false;
	}
	if (listTails.GetSize() == 0) {
		return false;
	}
	// the player only takes int damage, as it refer to the body length
	int damageVal = (int)std::round(damage);
	for (int i = 0; i < damageVal; ++i) {
		if (listTails.GetSize() > 0) {
			Remove1BodyNode();
		}
	}
	curHealth = listTails.GetSize();
	isInvulnerable = true;
	invulnerableCounter = invulnerableTime;

	if (listTails.GetSize() > 0) {
		OnHurt();
	}
	else {
		OnDie();
	}
	return true;
}

bool Player0::Heal(const float& val) {
	if (isDead || isPendingDestroy) {
		return false;
	}
	if (listTails.GetSize() == maxHealth) {
		return false;
	}
	// also the heal val should be int
	int healVal = (int)std::round(val);

	for (int i = 0; i < healVal; ++i) {
		if (listTails.GetSize() < maxHealth) {
			Add1BodyNode();
		}
	}
	curHealth = listTails.GetSize();
	OnHeal();
	return true;
}

void Player0::OnDie() {
	force = {};
	isDead = true;
}

std::string Player0::GetSpritePath()
{
	return "./Resources/Characters/red_character.png";
}

std::shared_ptr<BaseBodyNode> Player0::GetBodyNode(
	GamesEngineeringBase::Window& env, const Vector2& pos) {
	return std::make_shared<Player0TailNode>(env, pos, this, CharacterType::Player, BodyNodeType::Normal);
}