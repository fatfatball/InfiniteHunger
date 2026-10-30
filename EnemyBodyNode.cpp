#include "EnemyBodyNode.h"
#include "SmallStructs.h"

std::string EnemyBodyNode::GetSpritePath() const {
	switch (nodeType) {
	case BodyNodeType::Normal:
		return "./Resources/Characters/ship_B.png";
	case BodyNodeType::Heal:
		return "./Resources/Characters/tile_heart.png";
	}
}
