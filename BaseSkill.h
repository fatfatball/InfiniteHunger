#pragma once
class BasePlayer;

class BaseSkill {
protected:
	BasePlayer* owner;

public:
	BaseSkill(BasePlayer* owner) : owner(owner) {}

	virtual void CastSkill() = 0;
	virtual void Tick(const float& dt) {};
};

