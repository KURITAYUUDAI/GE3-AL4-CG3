#pragma once
#include "myMath.h"

class Player;

class IInputHandler;

class ICommand
{
public:

	virtual ~ICommand() = default;
	virtual void Execute(Player* player) = 0;
};

class MoveHorizontalCommand : public ICommand
{
public:
	MoveHorizontalCommand(IInputHandler* inputHandler) : inputHandler_(inputHandler) {}
	void Execute(Player* player) override;

private:
	IInputHandler* inputHandler_;
};

class ShotCommand : public ICommand
{
public:
	ShotCommand(IInputHandler* inputHandler) : inputHandler_(inputHandler) {}
	void Execute(Player* player) override;

private:
	IInputHandler* inputHandler_;
};

class AvoidCommand : public ICommand
{
public:
	AvoidCommand(IInputHandler* inputHandler) : inputHandler_(inputHandler) {}
	void Execute(Player* player) override;

private:
	IInputHandler* inputHandler_;
};