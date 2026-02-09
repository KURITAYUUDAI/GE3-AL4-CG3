#pragma once
#include "KamataEngine.h"
#include "myMath.h"

class Fade 
{
public:

	// フェードの状態
	enum class Status
	{
		None,		// フェードなし
		FadeIn,		// フェードイン中
		FadeOut,	// フェードアウト中
	};

	void Initialize();

	void Update();

	void Draw();

	void Start(Status status, float duration);

	void Stop();

	// フェード終了判定
	bool IsFinished() const;

private:

	// スプライトに使用するテクスチャ―ハンドル
	uint32_t textureHandle_;
	// フェードに使用するスプライト
	Sprite* sprite_ = nullptr;
	
	// 現在のフェードの状態
	Status status_ = Status::None;
	// フェードの持続時間
	float duration_ = 0.0f;
	// 経過時間カウンター
	float counter_ = 0.0f;

};
