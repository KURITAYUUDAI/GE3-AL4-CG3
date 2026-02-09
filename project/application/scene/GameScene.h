#pragma once
#include "KamataEngine.h"
#include "myMath.h"
#include <vector>
#include "Player.h"
#include "Skydome.h"
#include "CameraController.h"
#include "DeathParticles.h"
#include "Fade.h"
#include "BackGround.h"
#include "Enemy.h"
#include "Anchor.h"

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene 
{
public:
	// ゲームのフェーズ(型)
	enum class Phase
	{
		kFadeIn,	// フェードイン
		kPlay,		// ゲームプレイ
		kDeath,		// デス演出
		kFadeOut,	// フェードアウト
	};

	// デストラクタ
	~GameScene();

	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();

	// ワールドトランスフォーム
	std::vector<std::vector<WorldTransform*>> worldTransformBlocks_;

	// カメラ
	Camera camera_;

	// デバッグカメラ有効
	bool isDebugCameraActive_ = false;

	// デバッグカメラ
	DebugCamera* debugCamera_ = nullptr;

	// 自キャラ
	Player* player_ = nullptr;

	std::list<Enemy*> enemies_;

	void SetRespawnTimer();

	void RespawnEnemy();

	// 天球
	Skydome* skydome_ = nullptr;

	// 背景
	BackGround* backGround_ = nullptr;

	// カメラ操作
	CameraController* cameraController_ = nullptr;

	// 全ての当たり判定を行う
	void CheckAllCollisions();

	DeathParticles* deathParticles_ = nullptr;

	Phase phase_;

	void ChangePhase();

	// デスフラグのgetter
	bool IsFinished() const { return finished_; }

public:
	void PlaySEDecision();

private:

	// テクスチャハンドル
	uint32_t textureHandle_ = 0;

	// 自キャラモデル
	Model* modelPlayer_ = nullptr;

	// 自弾モデル
	Model* modelPlayerBullet_ = nullptr;

	// アンカーモデル
	Model* modelAnchor_ = nullptr;

	// 敵キャラモデル
	Model* modelEnemy_ = nullptr;

	// 敵弾モデル
	Model* modelEnemyBullet_ = nullptr;
	
	// 天球モデル
	Model* modelSkydome_ = nullptr;

	// 背景モデル
	Model* modelBackGround_ = nullptr;

	/*int32_t maxHitEffect_ = 3;*/

	// 死亡パーティクルモデル
	Model* modelDeathParticle_ = nullptr;

	// 終了フラグ
	bool finished_ = false;

	Fade* fade_ = nullptr;

	// 敵の最大数
	int maxEnemies_ = 6;

	// 敵のリスポーンタイマー
	std::list<float> respawnTimers_;

	//// テクスチャを読み込む
	uint32_t HowToOperate1Handle = TextureManager::Load("Sprite/HowToOperate1.png");
	uint32_t HowToOperate2Handle = TextureManager::Load("Sprite/HowToOperate2.png");
	uint32_t HowToOperate3Handle = TextureManager::Load("Sprite/HowToOperate3.png");
	uint32_t hitPointHandle = TextureManager::Load("Sprite/hitPoint.png");

	//// スプライトを生成
	Sprite* HowToOperate1Sprite_ = Sprite::Create(HowToOperate1Handle, {0.0f, 0.0f});
	Sprite* HowToOperate2Sprite_ = Sprite::Create(HowToOperate2Handle, {0.0f, 0.0f});
	Sprite* HowToOperate3Sprite_ = Sprite::Create(HowToOperate3Handle, {0.0f, 0.0f});
	
	std::vector<Sprite*> hitPointSprites_;

	//Sprite* EX1Sprite_ = Sprite::Create(EX1Handle, {0.0f, 0.0f});
	//Sprite* EX2Sprite_ = Sprite::Create(EX2Handle, {1090.0f, 0.0f});
	//Sprite* BGSprite_ = Sprite::Create(BGHandle, {0.0f, 0.0f});


};