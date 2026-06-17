#pragma once
#include "myMath.h"

class Enemy;

class AIHandler;

class IEnemyCommand
{
public:

	virtual ~IEnemyCommand() = default;
	virtual void Execute(Enemy* enemy) = 0;
};

class MoveCommand : public IEnemyCommand
{
public:
	MoveCommand(AIHandler* aiHandler) : aiHandler_(aiHandler) {}
	void Execute(Enemy* enemy) override;

private:
	AIHandler* aiHandler_;
};

class ShotCommand : public IEnemyCommand
{
public:
	ShotCommand(AIHandler* aiHandler) : aiHandler_(aiHandler) {}
	void Execute(Enemy* enemy) override;

private:
	AIHandler* aiHandler_;
};

class AttackCommand : public IEnemyCommand
{
public:
	AttackCommand(AIHandler* aiHandler) : aiHandler_(aiHandler) {}
	void Execute(Enemy* enemy) override;

private:
	AIHandler* aiHandler_;
};


