#include "PlayerState.h"
#include "GameSceneCommand.h"

void PlayerIdleState::Initialize(Player* player)
{
	(void)player;
}

void PlayerIdleState::Update(Player * player)
{
	(void)player;
}

void PlayerIdleState::Draw(Player * player)
{
	(void)player;
}

void PlayerIdleState::Finalize(Player * player)
{
	(void)player;
}


void PlayerMoveState::Initialize(Player* player)
{
	commands_.push_back(std::make_unique<MoveHorizontalCommand>(selector_.GetHandler()));
}

void PlayerMoveState::Update(Player* player)
{
	IInputHandler* handler = selector_.GetHandler();

	for (auto& command : commands_)
	{
		command->Execute(player);
	}
}

void PlayerMoveState::Draw(Player* player)
{
	(void)player;
}

void PlayerMoveState::Finalize(Player* player)
{
	(void)player;
}