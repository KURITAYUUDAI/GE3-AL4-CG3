#define NOMINMAX
#include "CameraController.h"
#include "Player.h"
#include "myMath.h"
#include <algorithm>
#include "SeedManager.h"

void CameraController::Initialize() 
{ 
	camera_.Initialize(); 
}

void CameraController::Update()
{
	//// 追従対象のワールドトランスフォームを参照
	//const WorldTransform& targetWorldTransform = target_;
	//// 追従対象とオフセットと追従対象の速度からカメラの目標座標を計算
	//destinationCamera_ = targetWorldTransform.translation_ + targetOffset_ + target_->GetVelocity() * kVelocityBias;

	//// 座標補間によりゆったり追従
	//camera_.translation_ = Lerp(camera_.translation_, destinationCamera_, kInterpolationRate);

	//float depth = -camera_.translation_.z; // Z 軸が負方向にオフセットされている想定

	//// fovY: カメラの垂直画角、aspect: アスペクト比
	//float halfV = std::tan(camera_.fovAngleY * 0.5f) * depth;
	//float halfH = halfV * camera_.aspectRatio;

	//// マップ全体の幅・高さ
	//float mapW = movableArea_.right - movableArea_.left;
	//float mapH = movableArea_.top - movableArea_.bottom;

	//// フラスタム半幅がマップ半幅を超えないように制限
	//halfH = std::min(halfH, mapW * 0.5f);
	//halfV = std::min(halfV, mapH * 0.5f);

	//float minX = movableArea_.left + halfH;
	//float maxX = movableArea_.right - halfH;
	//float minY = movableArea_.bottom + halfV;
	//float maxY = movableArea_.top - halfV;
	//
	//camera_.translation_.x = std::clamp(camera_.translation_.x, 
	//	targetWorldTransform.translation_.x + movingMargin.left, 
	//	targetWorldTransform.translation_.x + movingMargin.right);
	//camera_.translation_.y = std::clamp(camera_.translation_.y, 
	//	targetWorldTransform.translation_.y + movingMargin.bottom, 
	//	targetWorldTransform.translation_.y + movingMargin.top);

	//// 移動範囲制限
	//camera_.translation_.x = std::clamp(camera_.translation_.x, minX, maxX);
	//camera_.translation_.y = std::clamp(camera_.translation_.y, minY, maxY);

	if (isShake_)
	{
		px = -0.35f * float(frame) + 4.0f;
		if (px <= 0) 
		{
			isShake_ = false;
			shake_ = {0.0f, 0.0f, 0.0f};
			frame = 0;
		} 
		else 
		{
			frame++;
			shake_.x = SeedManager::GetInstance()->GenerateFloat(-px, px);
			shake_.y = SeedManager::GetInstance()->GenerateFloat(-px, px);
			shake_.z = 0.0f;
		}
	}
	

	camera_.translation_ = {0.0f, 0.0f, -20.0f};

	if (isShake_)
	{
		camera_.translation_ = Vector3{0.0f, 0.0f, -20.0f} + shake_;
	}


	// 行列を更新する
	camera_.UpdateMatrix();
}

void CameraController::Draw()
{

}

void CameraController::Reset()
{

	// 追従対象とオフセットからカメラの座標を計算
	camera_.translation_ = target_ + targetOffset_;
}

void CameraController::OnShake() 
{
	isShake_ = true;
}
