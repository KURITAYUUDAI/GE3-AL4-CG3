#pragma once
#include "KamataEngine.h"
#include "myMath.h"
#include "SpriteDraw.h"
#include <array>

class StageTime 
{
public:

	// シングルトンインスタンスの取得
	static StageTime* GetInstance();
	// 終了
	void Finalize();

public:

	void Initialize();

	void Update();

	void Draw();

	void SetTimeDigit();

	void AddScore();

	void SetScpreDigit();

	void ResultDraw();

public:

	const bool TimeUp();

	void PlaySETimeUp();

private: // シングルトンインスタンス
	static StageTime* instance_;

	StageTime() = default;
	~StageTime() = default;
	StageTime(StageTime&) = delete;
	StageTime& operator=(StageTime&) = delete;

	SpriteDraw* spriteDraw_ = SpriteDraw::GetInstance();

private:

	//// テクスチャを読み込む
	uint32_t timeHandle = TextureManager::Load("Sprite/Time.png");
	uint32_t scoreHandle = TextureManager::Load("Sprite/Score.png");
	uint32_t numberHandle = TextureManager::Load("SpriteSheet/number.png");
	uint32_t timeUpHandle = TextureManager::Load("Sprite/timeUp.png");

private:

	uint32_t timeUpSEDataHandle_;
	uint32_t timeUpSEHandle_;
	bool isPlayTimeUpSE_;


private:

	float time_;

	const float kMaxTime = 15.0f;

	std::array<int, 3> timeDigits_ {};
	
	// スプライトを生成
	Sprite* TimeSprite_ = nullptr;

	std::vector<Sprite*> numberTimeSprites_;

	Sprite* TimeUpSprite_ = nullptr;

	float timeUpTimer_;

	const float kMaxTimeUp = 2.0f;

	bool timeUp_ = false;

private:

	float score_;

	std::array<int, 7> scoreDigits_ {}; 

	Sprite* ScoreSprite_ = nullptr;

	std::vector<Sprite*> numberScoreSprites_;

};
