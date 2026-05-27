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
	virtual void Update(Player* player) = 0;
	virtual void Draw(Player* player) = 0;
	virtual void Finalize(Player* player) = 0;
};

class PlayerIdleState : public IPlayerState
{
public:
	void Initialize(Player* player) override;
	void Update(Player* player) override;
	void Draw(Player* player) override;
	void Finalize(Player* player) override;
};

class PlayerMoveState : public IPlayerState
{
	public:
	void Initialize(Player* player) override;
	void Update(Player* player) override;
	void Draw(Player* player) override;
	void Finalize(Player* player) override;

private:
	InputHandlerSelector selector_;

	std::vector<std::unique_ptr<ICommand>>      commands_;
};
