#include "Skydome.h"

using namespace KamataEngine;

void Skydome::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera) 
{
	
	// Skydomeの初期化処理を書く
	// 例えば、テクスチャの読み込みやモデルの生成など

	// NULLポインタチェック
	assert(model);

	model_ = model;

	camera_ = camera;

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
}

void Skydome::Update() 
{
	// Skydomeの更新処理を書く
	worldTransform_.rotation_.y += 0.001f; // ゆっくり回転

	if (worldTransform_.rotation_.y > pi) {
		worldTransform_.rotation_.y = worldTransform_.rotation_.y - 2.0f * pi;
	} 
	else if (worldTransform_.rotation_.y < -pi) 
	{
		worldTransform_.rotation_.y = worldTransform_.rotation_.y + 2.0f * pi;
	}

	// ワールド行列作成
	worldTransform_.matWorld_ = MakeAffineMatrixB(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);

	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();
}

void Skydome::Draw() 
{
	// Skydomeの描画処理を書く
	// 例えば、Skydomeのモデルを描画するなど

	model_->Draw(worldTransform_, *camera_);
	
}