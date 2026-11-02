#pragma once
#include "BaseCharacter.h"
#include "Scene.h"
#include "BaseBodyNode.h"
#include <memory>
#include "GamesEngineeringBase.h"
#include "BaseSkill.h"

class BasePlayer : public BaseCharacter {
protected:
	float physicalStiffness;
	float attackCooldown;
	float attackCooldownCounter;
	bool isFirstPlayer; // indicate that if this is the 1p, or 2p
	bool skillPressed;
	std::unique_ptr<BaseSkill> skill;
	friend class BaseSkill;
	int keyCount; // the number of keys the player has

public:
	BasePlayer(GamesEngineeringBase::Window& canvas, const bool& isFirst) : BaseCharacter(canvas) {
		physicalStiffness = 0.75f;
		mass = 15;
		maxHealth = 13;
		curHealth = 3;
		scale = Vector2(50, 50);
		globalPosition = { 0, 0 };
		rotation = 0.01f;
		maxVelocity = 400; // global position per second
		maxForce = 2000 * mass; // global position per second^2
		rotationSpeed = PI * 2.f; // radians per second
		renderOrder = RenderOrder::Third;
		prevState = { globalPosition, scale, rotation };
		curState = { globalPosition, scale, rotation };
		isFixed = true;
		canCollide = true;
		collisionRadius = 17;
		invulnerableTime = 0.5f;
		invulnerableCounter = invulnerableTime;
		isInvulnerable = false;
		attackCooldown = 0.75f;
		attackCooldownCounter = 0.f;
		isFirstPlayer = isFirst;
		skillPressed = false;
		keyCount = 0;
	}

	void OnStart() override {
		BaseCharacter::OnStart();

		auto ptrLine = std::make_shared<PolyLine2D>(env);
		ptrLine->lineColor = { 210, 210, 210, 255 };
		ptrLine->SetThickness(5);
		ptrLine->points.PushBack(globalPosition);
		polyLine2Ds.PushBack(ptrLine);

		List<BasePhysicalObject*> tmp;
		tmp.PushBack(this);
		linedNodes.PushBack(std::move(tmp));
		scene->AddObject(ptrLine);

		for (int i = 0; i < curHealth; ++i) {
			Add1BodyNode();
		}
	}

	void FixedUpdate(const float& dt) override {
		if (!isDead) {
			Vector2 inputDir = Vector2();
			if (isFirstPlayer) {
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

				if (env.keyPressed(0x43)) { // C
					Attack();
				}
				if (env.keyPressed(0x56) && skillPressed == false) { // V
					CastSkill();
				}
				skillPressed = env.keyPressed(0x56);
			}
			else {
				if (env.keyPressed(VK_UP)) {
					inputDir.y += -1;
				}
				if (env.keyPressed(VK_LEFT)) {
					inputDir.x += -1;
				}
				if (env.keyPressed(VK_DOWN)) {
					inputDir.y += 1;
				}
				if (env.keyPressed(VK_RIGHT)) {
					inputDir.x += 1;
				}

				if (env.keyPressed(VK_OEM_COMMA)) {
					Attack();
				}
				if (env.keyPressed(VK_OEM_PERIOD) && skillPressed == false) {
					CastSkill();
				}
				skillPressed = env.keyPressed(VK_OEM_PERIOD);
			}

			// the force calculation algorithm is still naive
			force = Vector2(std::cos(rotation), std::sin(rotation)) * maxForce * -inputDir.y;
			rotation = Math::WrapAngle(rotation + inputDir.x * rotationSpeed * dt);
		}

		// second player shouldn't be outside of the window
		if (!isDead && !scene->GetCameraAABB().Intersects(GetAABB(curState))) {
			if (!isFirstPlayer) {
				TeleportTo(scene->GetFirstPlayer()->globalPosition);
			}
		}

		skill->Tick(dt);
		TickAttackCooldown(dt);
		BaseCharacter::FixedUpdate(dt);
	}

	ColorRGBA GetColorAt(const float& u, const float& v) const override {
		int x = std::clamp(u, 0.f, 1.f) * sprite.width;
		int y = std::clamp(v, 0.f, 1.f) * sprite.height;
		x = x == sprite.width ? x - 1 : x;
		y = y == sprite.height ? y - 1 : y;
		auto ptr = sprite.atUnchecked(x, y);
		ColorRGBA color;
		color.r = ptr[0];
		color.g = ptr[1];
		color.b = ptr[2];
		if (isInvulnerable) {
			float tmp = invulnerableCounter / invulnerableTime * 3.f; // flash when invulnerable
			color.a = sprite.alphaAtUnchecked(x, y) * (tmp - floor(tmp));
		}
		else {
			color.a = sprite.alphaAtUnchecked(x, y);
		}
		return color;
	}

	unsigned char GetAlphaAt(const float& u, const float& v) const override {
		if (isInvulnerable) {
			return 127;
		}
		int x = std::clamp(u, 0.f, 1.f) * sprite.width;
		int y = std::clamp(v, 0.f, 1.f) * sprite.height;
		x = x == sprite.width ? x - 1 : x;
		y = y == sprite.height ? y - 1 : y;
		return sprite.alphaAtUnchecked(x, y);
	}

	bool TakeDamage(const float& damage) override {
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

	bool Heal(const float& val) override {
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

	void OnDie() override {
		force = {};
		isDead = true;
	}

	virtual bool Attack() {
		if (attackCooldownCounter > 0) {
			return false;
		}
		// do attack here
		attackCooldownCounter = attackCooldown;
		return true;
	}

	void TickAttackCooldown(const float& dt) {
		if (attackCooldownCounter > 0) {
			attackCooldownCounter -= dt;
		}
	}

	void SetInvulnerable(const float& time) {
		isInvulnerable = true;
		invulnerableCounter = time;
	}

	void AddKey() {
		++keyCount;
	}

	bool UseKey() {
		if (keyCount > 0) {
			--keyCount;
			return true;
		}
		return false;
	}

protected:
	void Add1BodyNode() {
		// no check for max health
		if (listTails.GetSize() == 0) {
			//auto ptr = std::make_shared<BaseBodyNode>(env, globalPosition + Vector2{ 1, 0 });
			auto ptr = GetBodyNode(env, globalPosition + Vector2{ 1, 0 });
			ptr->mass = 13;
			polyLine2Ds[0]->points.PushBack(globalPosition + Vector2{ 1, 0 });
			listTails.PushBack(ptr);
			scene->AddObject(ptr);
			linedNodes[0].PushBack(ptr.get());

			angleConstraints.PushBack(
				{ ConstraintType::DirectionAngle, this, ptr.get(), nullptr,
				PI, PI / 24.f , physicalStiffness });
			distConstraints.PushBack(
				{ ConstraintType::Distance, this, ptr.get(), nullptr,
				27.f, 0.f, physicalStiffness });
			rotationDefs.PushBack(
				{ RotationDefType::Prev, this, ptr.get(), nullptr });
		}
		else {
			// add new, and configure prev
			int last = listTails.GetSize() - 1;
			//auto ptr = std::make_shared<BaseBodyNode>(env, listTails[last]->globalPosition + Vector2{ 1, 0 });
			auto ptr = GetBodyNode(env, listTails[last]->globalPosition + Vector2{ 1, 0 });
			ptr->mass = std::clamp(13 - listTails.GetSize(), 1, 15);
			polyLine2Ds[0]->points.PushBack(listTails[last]->globalPosition + Vector2{ 1, 0 });
			listTails.PushBack(ptr);
			scene->AddObject(ptr);
			linedNodes[0].PushBack(ptr.get());

			if (last != 0) {
				angleConstraints.PushBack(
					{ ConstraintType::Angle, listTails[last - 1].get(), listTails[last].get(), ptr.get(),
					PI, PI / 12.f , physicalStiffness });
				rotationDefs[rotationDefs.GetSize() - 1] =
				{ RotationDefType::PrevAndNext, listTails[last - 1].get(), listTails[last].get(), ptr.get() };
			}
			else {
				angleConstraints.PushBack(
					{ ConstraintType::Angle, this, listTails[last].get(), ptr.get(),
					PI, PI / 12.f , physicalStiffness });
				rotationDefs[rotationDefs.GetSize() - 1] =
				{ RotationDefType::PrevAndNext, this, listTails[last].get(), ptr.get() };
			}

			distConstraints.PushBack(
				{ ConstraintType::Distance, listTails[last].get(), ptr.get(), nullptr,
				17.f, 0.f, physicalStiffness });
			rotationDefs.PushBack(
				{ RotationDefType::Prev, listTails[last].get(), ptr.get(), nullptr });
		}
	}

	void Remove1BodyNode() {
		int size = listTails.GetSize();
		if (size == 0) {
			return;
		}

		// remove last, configure last - 1
		distConstraints.PopBack();
		angleConstraints.PopBack();
		if (rotationDefs.GetSize() >= 2) {
			rotationDefs[rotationDefs.GetSize() - 2].type = RotationDefType::Prev;
			rotationDefs[rotationDefs.GetSize() - 2].o3 = nullptr;
		}
		rotationDefs.PopBack();

		polyLine2Ds[0]->points.PopBack();
		linedNodes[0].PopBack();

		listTails[size - 1]->isActive = false;
		listTails[size - 1]->isPendingDestroy = true;
		listTails.PopBack();

		curHealth = listTails.GetSize();
	}

	virtual std::shared_ptr<BaseBodyNode> GetBodyNode(GamesEngineeringBase::Window& env, const Vector2& pos) = 0;

	void CastSkill() {
		TakeDamageNoInvulnerable(1.f);
		skill->CastSkill();
	}

	bool TakeDamageNoInvulnerable(const float& damage) {
		if (isDead || isPendingDestroy) {
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

		if (listTails.GetSize() > 0) {
			OnHurt();
		}
		else {
			OnDie();
		}

		return true;
	}
};