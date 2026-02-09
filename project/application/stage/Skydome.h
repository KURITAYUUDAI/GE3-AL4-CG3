#pragma once
#include "KamataEngine.h"
#include "myMath.h"

class Skydome 
{
public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera);

	/// <summary>
	/// 初期化
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private:

	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;
	
	// カメラ
	KamataEngine::Camera* camera_ = nullptr;
	
	// モデル
	KamataEngine::Model* model_ = nullptr;


};
