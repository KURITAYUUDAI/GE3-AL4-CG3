#pragma once
#include "KamataEngine.h"
#include "myMath.h"
#include <array>
#include <numbers>

using namespace KamataEngine;

class DeathParticles 
{
public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name = "model">モデル</param>
	/// <param name = "textureHandle">テクスチャハンドル</param>
	void Initialize(Model* model, Camera* camera, const Vector3& position);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	bool GetIsFinished() { return isFinished_; }

private:

	// モデル
	Model* model_ = nullptr;

	// カメラ
	Camera* camera_ = nullptr;

	// パーティクルの個数
	static inline const uint32_t kNumParticles = 8;

	std::array<WorldTransform, kNumParticles> worldTransforms_;

	// 存続時間（消滅までの時間）<秒>
	static inline const float kDuration = 2.0f;
	// 移動の速さ
	static inline const float kSpeed = 0.1f;
	// 分割した1個分の角度
	static inline const float kAngleUint = 2.0f* std::numbers::pi_v<float> / kNumParticles;

	// 終了フラグ
	bool isFinished_ = false;
	// 経過時間カウント
	float counter_ = 0.0f; 

	// 色変更オブジェクト
	ObjectColor objectColor_;

	// 色の数値
	Vector4 color_;

};
