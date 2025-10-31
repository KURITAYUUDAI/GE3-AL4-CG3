#pragma once
#include <chrono>
#include <thread>

class FixFPS
{
public:

	// FPS固定初期化
	void InitializeFixFPS(const float& frameNum);

	// FPS固定更新
	void UpdateFixFPS();

	// フレーム枚数のセッター
	void SetFrameNum(const float& frameNum){ frameNum_ = frameNum; }

	// フレーム枚数のゲッター
	float GetFrameNum(){ return frameNum_; }

private:

	// 1秒間に表示するフレームの枚数
	float frameNum_ = 60.0f;

	// 記録時間
	std::chrono::steady_clock::time_point reference_;

};