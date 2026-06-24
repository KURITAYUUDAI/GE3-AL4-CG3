#pragma once
#include <Windows.h>
#include <cmath>
#include <chrono>

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

	void Start();
	void End();

	//void Sleep()

public:

	const LONGLONG& GetTime(){ return duratetionSec_; }

private:

	std::chrono::steady_clock::time_point start_;
	std::chrono::steady_clock::time_point end_;

	LONGLONG duratetionSec_ = 0;
};

