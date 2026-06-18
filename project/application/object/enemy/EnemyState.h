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



