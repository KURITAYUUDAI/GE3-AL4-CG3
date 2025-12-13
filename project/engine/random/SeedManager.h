#pragma once
#include <random>

class SeedManager
{
public:

	// シングルトンインスタンスの取得
	static SeedManager* GetInstance();
	// 終了
	void Finalize();

public:

	// 初期化
	void Initialize();

public: // 外部入出力

	// intで乱数生成
	int GenerateInt(int min, int max);
	// floatで乱数生成
	float GenerateFloat(float min, float max);

private:	// シングルトンインスタンス

	static SeedManager* instance_;

	SeedManager() = default;
	~SeedManager() = default;
	SeedManager(SeedManager&) = delete;
	SeedManager& operator=(SeedManager&) = delete;

private:

	std::mt19937 randomEngine_;

};

