#include "EnemyState.h"
#include "Enemy.h"
#include "EnemyCommand.h"

void EnemyIdleState::Initialize(Enemy* enemy)
{
	moveCommand_ = std::make_unique<MoveCommand>(
		enemy->GetAIHandler());
	shotCommand_ = std::make_unique<ShotCommand>(
		enemy->GetAIHandler());
}

void EnemyIdleState::Update(Enemy* enemy, const float& deltaTime)
{
	AIHandler* handler = enemy->GetAIHandler();

	moveCommand_->Execute(enemy);
	if (handler->IsShot(deltaTime))
	{
		shotCommand_->Execute(enemy);
	}
}

void EnemyIdleState::Draw(Enemy * enemy)
{
	(void)enemy;
}

void EnemyIdleState::Finalize(Enemy * enemy)
{
	(void)enemy;
}


void EnemyMoveState::Initialize(Enemy* enemy)
{
	(void)enemy;
}

void EnemyMoveState::Update(Enemy * enemy, const float& deltaTime)
{
	(void)enemy;
}

void EnemyMoveState::Draw(Enemy * enemy)
{
	(void)enemy;
}

void EnemyMoveState::Finalize(Enemy * enemy)
{
	(void)enemy;
}



void EnemyShotState::Initialize(Enemy* enemy)
{
	moveCommand_ = std::make_unique<MoveCommand>(
		enemy->GetAIHandler());
}

void EnemyShotState::Update(Enemy * enemy, const float& deltaTime)
{
	moveCommand_->Execute(enemy);

	timer_ += deltaTime;
	if (timer_ >= duration_)
	{
		enemy->ChangeState(std::make_unique<EnemyIdleState>());
	}
}

void EnemyShotState::Draw(Enemy * enemy)
{
	(void)enemy;
}

void EnemyShotState::Finalize(Enemy * enemy)
{
	(void)enemy;
}
