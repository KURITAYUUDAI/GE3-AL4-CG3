#include "BackGround.h"

using namespace KamataEngine;

void BackGround::Initialize(Model* model, Camera* camera, const Vector3& position)
{

	// Skydomeの初期化処理を書く
	// 例えば、テクスチャの読み込みやモデルの生成など

	// NULLポインタチェック
	assert(model);

	model_ = model;

	camera_ = camera;

	// 位置設定
	worldTransform_.translation_ = position;

	// スケール設定
	worldTransform_.scale_ = {0.3f, 0.3f, 0.3f};

	// 回転設定
	worldTransform_.rotation_ = {0.0f, 90.0f / 180.0f * std::numbers::pi_v<float>, 0.0f};

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
}

void BackGround::Update() 
{
	// ワールド行列作成
	worldTransform_.matWorld_ = MakeAffineMatrixB(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);

	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();
}

void BackGround::Draw() 
{
	model_->Draw(worldTransform_, *camera_);
}