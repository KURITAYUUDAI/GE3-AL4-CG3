#include "FixFPS.h"

void FixFPS::InitializeFixFPS(const float& frameNum)
{
	frameNum_ = frameNum;

	reference_ = std::chrono::steady_clock::now();
}


void FixFPS::UpdateFixFPS()
{
	// 1/60秒ぴったりの時間
	const std::chrono::microseconds kMinTime(uint64_t(1.0e+6f / frameNum_));

	// 1/60秒よりわずかに短い時間
	const std::chrono::microseconds kMinCheckTime(uint64_t(1.0e+6f / (frameNum_ * 1.05f)));

	// 現在時間を取得する
	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
	// 前回記録からの経過時間を取得する
	std::chrono::microseconds elapsed =
		std::chrono::duration_cast<std::chrono::microseconds>(now - reference_);

	// 1/60秒（よりわずかに短い時間）立っていない場合
	if (elapsed < kMinCheckTime)
	{
		// 1/60秒経過するまで微小なスリープを繰り返す
		while (std::chrono::steady_clock::now() - reference_ < kMinTime)
		{
			// 1マイクロ秒スリープ
			std::this_thread::sleep_for(std::chrono::microseconds(1));
		}
	}

	// 現在の時間を記録する
	reference_ = std::chrono::steady_clock::now();

}
