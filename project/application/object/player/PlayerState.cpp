#include "PlayerState.h"
#include "Player.h"
#include "GameSceneCommand.h"

void PlayerIdleState::Initialize(Player* player)
{
	moveCommand_ = std::make_unique<MoveHorizontalCommand>(
		player->GetInputHandlerSelector()->GetHandler());
	shotCommand_ = std::make_unique<ShotCommand>(
		player->GetInputHandlerSelector()->GetHandler());
}

void PlayerIdleState::Update(Player * player, const float& deltaTime)
{
	IInputHandler* handler = player->GetInputHandlerSelector()->GetHandler();

	moveCommand_->Execute(player);
	if (handler->IsActionPressed("shot"))
	{
		shotCommand_->Execute(player);
	}
}

void PlayerIdleState::Draw(Player* player)
{
	(void)player;
}

void PlayerIdleState::Finalize(Player* player)
{
	(void)player;
}

void PlayerShotState::Initialize(Player* player)
{
	moveCommand_ = std::make_unique<MoveHorizontalCommand>(
		player->GetInputHandlerSelector()->GetHandler());
}

void PlayerShotState::Update(Player* player, const float& deltaTime)
{
	moveCommand_->Execute(player);

	timer_ += kDeltaTime;
	if (timer_ >= duration_)
	{
		player->ChangeState(std::make_unique<PlayerIdleState>());
	}
}

void PlayerShotState::Draw(Player* player)
{
	(void)player;
}

void PlayerShotState::Finalize(Player* player)
{
	(void)player;
}