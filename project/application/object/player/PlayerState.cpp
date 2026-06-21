#include "PlayerState.h"
#include "Player.h"

void PlayerIdleState::Initialize(Player* player)
{
	moveCommand_ = std::make_unique<MoveHorizontalCommand>(
		player->GetInputHandlerSelector()->GetHandler());
	shotCommand_ = std::make_unique<ShotCommand>(
		player->GetInputHandlerSelector()->GetHandler());
	avoidCommand_ = std::make_unique<AvoidCommand>(
		player->GetInputHandlerSelector()->GetHandler());
}

void PlayerIdleState::Update(Player * player, const float& deltaTime)
{
	IInputHandler* handler = player->GetInputHandlerSelector()->GetHandler();

	moveCommand_->Execute(player);
	
	if (handler->IsActionTriggerd("shot"))
	{
		shotCommand_->Execute(player);
		return; // ★ 追加: Shot()内部でChangeStateされる
	}
	if (handler->IsActionTriggerd("avoid"))
	{
		avoidCommand_->Execute(player);
		return; // ★ 追加: Avoid()内部でChangeStateされる
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
	IInputHandler* handler = player->GetInputHandlerSelector()->GetHandler();

	moveCommand_->Execute(player);

	timer_ += deltaTime;
	if (timer_ >= duration_)
	{
		player->ClearLockOn();
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

void PlayerAvoidState::Initialize(Player* player)
{
	if (inputDirection_.x != 0.0f && inputDirection_.y != 0.0f)
	{
		Vector3 currentVelocity = player->GetVelocity();
		if (currentVelocity.x != 0.0f || currentVelocity.y != 0.0f)
		{
			// 入力方向に回避速度を加算
			avoidDirection_ = currentVelocity;
		}
		else
		{
			avoidDirection_ = {1.0f, 0.0f, 0.0f};
		}
	}
	else
	{
		avoidDirection_ = { inputDirection_.x, inputDirection_.y, 0.0f };
	}

	if (inputDirection_.x != 0.0f || inputDirection_.y != 0.0f)
	{
		avoidDirection_ = Normalize(avoidDirection_);
	}

	//float maxAvoidRoll = 2.0f * pi; // 最大で傾ける角度（ラジアン。0.5f は約30度）

	//if (avoidDirection_.x < 0.0f) 
	//{
	//	player->SetTargetRoll({ 0.0f, 0.0f, maxAvoidRoll});  // 左回避時のロール
	//} 
	//else // if (avoidDirection_.x > 0.0f) 
	//{
	//	player->SetTargetRoll({0.0f, 0.0f, -maxAvoidRoll}); // 右回避時のロール
	//}
	
}

void PlayerAvoidState::Update(Player* player, const float& deltaTime)
{
	IInputHandler* handler = player->GetInputHandlerSelector()->GetHandler();

	timer_ += deltaTime;

	float progress = timer_ / duration_;
	if (progress > 1.0f) progress = 1.0f;

	float oneRotation = 2.0f * pi;
	float currentRoll = 0.0f;

	if (avoidDirection_.x < 0.0f) 
	{
		currentRoll = progress * oneRotation; // 左回転
	}
	else 
	{
		currentRoll = -progress * oneRotation;  // 右回転
	}

	Vector3 currentRotate = player->GetRotate();
	currentRotate.z = currentRoll;
	player->SetRotate(currentRotate);

	float speedRate = 1.0f - (progress * progress);

	float currentSpeed = avoidSpeed_ * speedRate;

	player->MoveAvoid(avoidDirection_, currentSpeed);

	/*if (timer_ / duration_ > 0.8f)
	{
		player->SetTargetRoll({0.0f, 0.0f, 0.0f});
	}*/
	
	if (timer_ >= duration_)
	{
		player->SetRotate(Vector3(0.0f, 0.0f, 0.0f));
		
		player->ClearLockOn();
		player->ChangeState(std::make_unique<PlayerIdleState>());
		
	}
}

void PlayerAvoidState::Draw(Player* player)
{
	(void)player;
}

void PlayerAvoidState::Finalize(Player * player)
{
	/*player->SetTargetRoll({0.0f, 0.0f, 0.0f});*/
	
}

