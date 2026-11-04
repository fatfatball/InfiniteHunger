#include "EnemyBodyNode.h"
#include "SmallStructs.h"
#include "BasePlayer.h"

std::string EnemyBodyNode::GetSpritePath() const {
	switch (nodeType) {
	case BodyNodeType::Normal:
		return "./Resources/Characters/ship_B.png";
	case BodyNodeType::Heal:
		return "./Resources/Characters/tile_heart.png";
	}
}

EnemyBodyNode::EnemyBodyNode(GamesEngineeringBase::Window& canvas, const Vector2& pos,
	BaseCharacter* owner, const CharacterType& type, const BodyNodeType& nodeType) :
	BaseBodyNode(canvas, pos, owner, type, nodeType) {
	if (nodeType == BodyNodeType::Heal) {
		collisionRadius = 6.f;
	}
}

void EnemyBodyNode::OnCollision(BaseGameObject* other, const Vector2& normal, const Vector2& position) {
	if (nodeType != BodyNodeType::Heal || !isOwnerDead) {
		return;
	}
	auto ptr = dynamic_cast<BasePlayer*>(other);
	if (ptr && ptr->Heal(1.f)) {
		isActive = false;
		isPendingDestroy = true;
	}
}