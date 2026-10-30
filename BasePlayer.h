#pragma once
#include "BaseCharacter.h"
#include "Scene.h"
#include "BaseBodyNode.h"
#include <memory>
#include "GamesEngineeringBase.h"

class BasePlayer : public BaseCharacter {
protected:
	float physicalStiffness;

public:
	BasePlayer(GamesEngineeringBase::Window& canvas) : BaseCharacter(canvas) {
		physicalStiffness = 0.75f;
		mass = 15;
		maxHealth = 13;
		curHealth = 3;
		scale = Vector2(50, 50);
		globalPosition = { 0, 0 };
		rotation = 0.01f;
		maxVelocity = 400; // global position per second
		maxForce = 1000 * mass; // global position per second^2
		rotationSpeed = PI * 3.f / 2.f; // radians per second
		renderOrder = RenderOrder::Third;
		prevState = { globalPosition, scale, rotation };
		curState = { globalPosition, scale, rotation };
		isFixed = true;
		canCollide = true;
		collisionRadius = 17;
		invulnerableTime = 0.5f;
		invulnerableCounter = invulnerableTime;
		isInvulnerable = false;
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
		// second player shouldn't be outside of the window
		if (!scene->GetCameraAABB().Intersects(GetAABB(curState))) {
			TeleportTo(scene->GetFirstPlayer()->globalPosition);
		}
		BaseCharacter::FixedUpdate(dt);
	}

	void TakeDamage(const float& damage) override {
		if (isDead || isInvulnerable || isPendingDestroy) {
			return;
		}
		if (listTails.GetSize() == 0) {
			return;
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
	}

	void Heal(const float& val) override {
		if (isDead || isPendingDestroy) {
			return;
		}
		if (listTails.GetSize() == maxHealth) {
			return;
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
	}
	// OnHurt, OnHeal, OnDie

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
				PI, PI / 12.f , physicalStiffness });
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
};