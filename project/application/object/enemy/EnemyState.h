#pragma once
#include "myMath.h"
#include <memory>
#include "AIHandler.h"
#include "EnemyCommand.h"

class Enemy;

class IEnemyState
{
public:
	virtual void Initialize(Enemy* enemy) = 0;
	virtual void Update(Enemy* enemy, const float& deltaTime) = 0;
	virtual void Draw(Enemy* enemy) = 0;
	virtual void Finalize(Enemy* enemy) = 0;
};

class EnemyIdleState : public IEnemyState
{
public:
	void Initialize(Enemy* enemy) override;
	void Update(Enemy* enemy, const float& deltaTime) override;
	void Draw(Enemy* enemy) override;
	void Finalize(Enemy* enemy) override;

private:

	std::unique_ptr<IEnemyCommand> moveCommand_;
	std::unique_ptr<IEnemyCommand> shotCommand_;
	std::unique_ptr<IEnemyCommand> attackCommand_;
};

class EnemyMoveState : public IEnemyState
{
public:
	void Initialize(Enemy* enemy) override;
	void Update(Enemy* enemy, const float& deltaTime) override;
	void Draw(Enemy* enemy) override;
	void Finalize(Enemy* enemy) override;
};

class EnemyShotState : public IEnemyState
{
public:
	void Initialize(Enemy* enemy) override;
	void Update(Enemy* enemy, const float& deltaTime) override;
	void Draw(Enemy* enemy) override;
	void Finalize(Enemy* enemy) override;

private:

	float timer_ = 0.0f;
	float duration_ = 0.1f;

	std::unique_ptr<IEnemyCommand> moveCommand_;
};

class EnemyAttackState : public IEnemyState
{
private:
	enum class AttackPhase
	{
		Approach,
		Homing,
		Lock,
		Attack,
		Away,
	};

public:

	void Initialize(Enemy* enemy) override;
	void Update(Enemy* enemy, const float& deltaTime) override;
	void Draw(Enemy* enemy) override;
	void Finalize(Enemy* enemy) override;

private:

	AttackPhase attackPhase_ = AttackPhase::Approach;

	Vector3 beforePosition_ = { 0.0f, 0.0f, 0.0f };
	Vector3 approachPosition_ = { 0.0f, 0.0f, 20.0f };
	Vector3 handPosition_ = { 0.0f, 0.0f, 0.0f };
	Vector3 homingPosition_ = { 0.0f, 0.0f, 0.0f };

	float timer_ = 0.0f;
	float duration_ = 0.5f;
	std::unique_ptr<IEnemyCommand> attackCommand_;
};
