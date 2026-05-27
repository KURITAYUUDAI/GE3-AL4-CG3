#pragma once
#include "DirectXBase.h"
#include "object3d.h"
#include "myMath.h"
#include "Model.h"
#include <memory>

#include "PlayerState.h"

class Player
{
public:

	void Initialize();
	void Update();
	void Draw();
	void Finalize();

	void ChangeState(std::unique_ptr<IPlayerState> newState);

public:	// Command

	void MoveHorizontal(const float& directionX, const float& directionY);
	void Decelerate();


public:	//外部入出力

	void SetEnvironmentTextureIndex(const uint32_t& srvIndex){ environmentTextureIndex_ = srvIndex; }

	
	

private:

	// 現在の状態
	std::unique_ptr<IPlayerState> state_;
	
	std::string psoName_ = "Environment";
	PSOManager::BlendMode blendMode_ = PSOManager::BlendMode::Normal;
	PSOManager::FillMode fillMode_ = PSOManager::FillMode::kSolid;

	std::unique_ptr<Object3d> object3d_;

	Transform transform_;

	Vector3 velocity_ = { 0.0f, 0.0f, 0.0f };
	Vector3 maxSpeed_ = { 0.2f, 0.2f, 0.2f }; // スティック全倒し時の最高速度
	float lerpFactor_ = 0.2f;                  // 追従の滑らかさ（0〜1）

	// 環境テクスチャのsrvIndex
	uint32_t environmentTextureIndex_ = 0;

	bool isDraw_ = true;
};