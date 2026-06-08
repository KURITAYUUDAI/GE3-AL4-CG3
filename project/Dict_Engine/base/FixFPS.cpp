#include "FixFPS.h"

void FixFPS::Initialize(const float& frameNum)
{
	frameNum_ = frameNum;

	reference_ = std::chrono::steady_clock::now();

	deltaTime_ = 1.0f / frameNum_;
}


void FixFPS::Update()
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
		//// 1/60秒経過するまで微小なスリープを繰り返す
		//while (std::chrono::steady_clock::now() - reference_ < kMinTime)
		//{
		//	// 1マイクロ秒スリープ
		//	std::this_thread::sleep_for(std::chrono::microseconds(1));
		//}

		// 残り時間が2ミリ秒以上あれば sleep_for でCPUを休ませる
		std::chrono::microseconds remaining = kMinTime - elapsed;
		if (remaining > std::chrono::milliseconds(2))
		{
			std::this_thread::sleep_for(remaining - std::chrono::milliseconds(1));
		}

		// 残りのわずかな時間は yield で精密に合わせる
		while (std::chrono::steady_clock::now() - reference_ < kMinTime)
		{
			std::this_thread::yield();
		}
	}

	//// 現在の時間を記録する
	//reference_ = std::chrono::steady_clock::now();

	std::chrono::steady_clock::time_point endOfFrame = std::chrono::steady_clock::now();
	std::chrono::duration<float> actualElapsed = endOfFrame - reference_;

	// 秒単位に変換して保持 (例: 60fpsで安定していれば 0.01666... が入る)
	deltaTime_ = actualElapsed.count();

	// 3. 次のフレームのための基準時間更新（ドリフト対策）
	if (endOfFrame - reference_ > kMinTime * 2)
	{
		reference_ = endOfFrame; // 処理落ち時はリセット
	} 
	else
	{
		reference_ += kMinTime; // 通常時は理想時間を足していく
	}

}
