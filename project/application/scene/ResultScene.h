#pragma once
#include "KamataEngine.h"
#include "myMath.h"
#include "Fade.h"
#include "Skydome.h"

class ResultScene 
{
public:

	public:
	// ゲームのフェーズ(型)
	enum class Phase {
		kFadeIn,  // フェードイン
		kMain,    // メイン部
		kFadeOut, // フェードアウト
	};

	~ResultScene();

	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();

	// デスフラグのgetter
	bool IsFinished() const { return finished_; }

	// カメラ
	Camera camera_;

	// 天球
	Skydome* skydome_ = nullptr;

public:
	void PlaySEDecision();

private:

	// 終了フラグ
	bool finished_ = false;

	Fade* fade_ = nullptr;

	Phase phase_ = Phase::kFadeIn;

	// テクスチャを読み込む
	/*uint32_t TEX1Handle = TextureManager::Load("Sprite/TEX1.png");*/
	uint32_t TEX2Handle = TextureManager::Load("Sprite/TEX2.png");
	uint32_t gameOverHandle = TextureManager::Load("Sprite/gameOver.png");

	// Sprite作成
	/*Sprite* TEX1Sprite_ = Sprite::Create(TEX1Handle, {0.0f, 0.0f});*/
	Sprite* TEX2Sprite_ = Sprite::Create(TEX2Handle, {0.0f, 0.0f});
	Sprite* gameOverSprite_ = Sprite::Create(gameOverHandle, {0.0f, 0.0f});

	// 自キャラモデル
	Model* modelPlayer_ = nullptr;

	WorldTransform worldTransformPlayer_;

	// 天球モデル
	Model* modelSkydome_ = nullptr;

private:
	uint32_t decisionSEDataHandle_;
	uint32_t decisionSEHandle_;
	bool isPlayDecisionSE_;

};
