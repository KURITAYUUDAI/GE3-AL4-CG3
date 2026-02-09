#include "DeathParticles.h"

using namespace KamataEngine;

void DeathParticles::Initialize(Model* model, Camera* camera, const Vector3& position)
{
	// プレイヤーの初期化処理を書く

	// NULLポインタチェック
	assert(model);

	model_ = model;

	camera_ = camera;

	// ワールド変換の初期化
	for (WorldTransform& worldTransform : worldTransforms_)
	{
		worldTransform.Initialize();
		worldTransform.translation_ = position;
	}

	isFinished_ = false;

	objectColor_.Initialize();
	color_ = { 1.0f, 1.0f, 1.0f, 1.0f };
}

void DeathParticles::Update()
{
	if (isFinished_) 
	{
		return;
	}

	counter_ += 1.0f / 60.0f;

	// 存続時間の上限に達したら
	if (counter_ >= kDuration)
	{
		counter_ = kDuration;

		//　終了扱いにする
		isFinished_ = true;
	}

	color_.w = std::clamp(kDuration - counter_, 0.0f, 1.0f);
	// 色を変更オブジェクトに色の数値を設定する
	objectColor_.SetColor(color_);

	//カウンターを1フレームの分の秒数進める 
	for (int32_t i = 0; i < kNumParticles; ++i)
	{
		// 基本となる速度ベクトル
		Vector3 velocity = {kSpeed, 0.0f, 0.0f};
		// 回転角を計算する
		float angle = kAngleUint * i; 
		// Z軸まわりの回転行列
		Matrix4x4 matrixRotation = MakeRotateZMatrix(angle);
		// 基本ベクトルを回転させて速度ベクトルを得る
		velocity = Transform(velocity, matrixRotation);
		// 移動処理
		worldTransforms_[i].translation_ += velocity;
	}


	for (WorldTransform& worldTransform : worldTransforms_)
	{

		worldTransform.matWorld_ = MakeAffineMatrixB(
			worldTransform.scale_, worldTransform.rotation_, worldTransform.translation_);
		worldTransform.TransferMatrix();
	}

}

void DeathParticles::Draw()
{
	if (isFinished_)
	{
		return;
	}

	for (WorldTransform& worldTransform : worldTransforms_) 
	{
		// オブジェクトカラーを nullptr に設定して描画
		model_->Draw(worldTransform, *camera_, &objectColor_);
	}
}