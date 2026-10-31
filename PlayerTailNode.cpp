#include "PlayerTailNode.h"
#include "GamesEngineeringBase.h"

std::string PlayerTailNode::GetSpritePath() const
{
	switch (playerType) {
	case PlayerType::Swordsman:
		return "./Resources/Characters/red_hand.png";
	case PlayerType::Archer:
		return "./Resources/Characters/yellow_hand.png";
	case PlayerType::Wizard:
		return "./Resources/Characters/purple_hand.png";
	default:
		return "./Resources/Characters/red_hand.png";
	}
}