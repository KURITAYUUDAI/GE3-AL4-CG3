#pragma once
#include <Windows.h>
#include <cmath>
#include <chrono>
#include <thread>

class ChronoManager
{
public:

	// シングルトンインスタンスの取得
	static ChronoManager* GetInstance();
	// 終了
	void Finalize();

	// コンストラクタに渡すための鍵
	class ConstructorKey
	{
	private:
		ConstructorKey() = default;
		friend class ChronoManager;
	};

	// PassKeyを受け取るコンストラクタ
	explicit ChronoManager(ConstructorKey){}

private:

	// unique_ptr の型定義に Deleter を入れることでdeleteが可能になる
	static std::unique_ptr<ChronoManager> instance_;

	~ChronoManager() = default;
	ChronoManager(ChronoManager&) = delete;
	ChronoManager& operator=(ChronoManager&) = delete;

	friend struct std::default_delete<ChronoManager>;

public:

	// FPS固定初期化
	void Initialize(const float& frameNum);

	// FPS固定更新
	void Update();

	void Start();
	void End();

	//void Sleep()

public:

	// フレーム枚数のセッター
	void SetFrameNum(const float& frameNum){ frameNum_ = frameNum; }

	// フレーム枚数のゲッター
	float GetFrameNum(){ return frameNum_; }

	// デルタタイムのゲッター
	float GetDeltaTime() { return deltaTime_; }

	const LONGLONG& GetTime(){ return duratetionMiliSec_; }

private:

	// 1秒間に表示するフレームの枚数
	float frameNum_ = 60.0f;

	// 記録時間
	std::chrono::steady_clock::time_point reference_;

	// deltaTime
	float deltaTime_ = 0.0f;

	std::chrono::steady_clock::time_point start_;
	std::chrono::steady_clock::time_point end_;

	LONGLONG duratetionMiliSec_ = 0;
};

