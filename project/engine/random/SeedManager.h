#pragma once
#include <random>
#include <memory>

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

	// unique_ptr が delete するために使用する構造体
	struct Deleter
	{
		void operator()(SeedManager* p) const
		{
			// クラス内部のスコープなので private なデストラクタを呼べる
			delete p;
		}
	};

	// unique_ptr の型定義に Deleter を入れることでdeleteが可能になる
	static std::unique_ptr<SeedManager, Deleter> instance_;

	SeedManager() = default;
	~SeedManager() = default;
	SeedManager(SeedManager&) = delete;
	SeedManager& operator=(SeedManager&) = delete;

private:

	std::mt19937 randomEngine_;

};

