#pragma once
class BasePlayer;

// it's more like an interface, for players to cast skills
class BaseSkill {
protected:
	BasePlayer* owner;

public:
	BaseSkill(BasePlayer* owner) : owner(owner) {}
	
	virtual ~BaseSkill() {}
	virtual void CastSkill() = 0;
	virtual void Tick(const float& dt) {};
};

