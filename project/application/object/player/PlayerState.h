#pragma once
#include "myMath.h"
#include <memory>
#include "InputHandlerSelector.h"
#include "GameSceneCommand.h"

class Player;

class IPlayerState
{
public:
	virtual void Initialize(Player* player) = 0;
	virtual void Update(Player* player, const float& deltaTime) = 0;
	virtual void Draw(Player* player) = 0;
	virtual void Finalize(Player* player) = 0;
};

class PlayerIdleState : public IPlayerState
{
public:
	void Initialize(Player* player) override;
	void Update(Player* player, const float& deltaTime) override;
	void Draw(Player* player) override;
	void Finalize(Player* player) override;

private:

	std::unique_ptr<IPlayerCommand>      moveCommand_;
	std::unique_ptr<IPlayerCommand>      shotCommand_;
	std::unique_ptr<IPlayerCommand>      avoidCommand_;
};

class PlayerShotState : public IPlayerState
{
public:
	void Initialize(Player* player) override;
	void Update(Player* player, const float& deltaTime) override;
	void Draw(Player* player) override;
	void Finalize(Player* player) override;

private:
	float timer_ = 0.0f;
	float duration_ = 0.1f;

	std::unique_ptr<IPlayerCommand>      moveCommand_;
};

class PlayerAvoidState : public IPlayerState
{
public:

	PlayerAvoidState(const Vector2& direction) : inputDirection_(direction) {}
	
	void Initialize(Player* player) override;
	void Update(Player* player, const float& deltaTime) override;
	void Draw(Player* player) override;
	void Finalize(Player* player) override;

private:
	float timer_ = 0.0f;
	float duration_ = 0.5f;

	Vector3 avoidDirection_ = { 0.0f, 0.0f, 0.0f };
	Vector2 inputDirection_ = { 0.0f, 0.0f };
	float avoidSpeed_ = 25.0f; // 回避の速度

};
