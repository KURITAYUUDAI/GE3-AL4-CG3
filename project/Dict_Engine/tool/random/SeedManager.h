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

	// コンストラクタに渡すための鍵
	class ConstructorKey
	{
	private:
		ConstructorKey() = default;
		friend class SeedManager;
	};

	// PassKeyを受け取るコンストラクタ
	explicit SeedManager(ConstructorKey){}

private: 	// シングルトンインスタンス

	static std::unique_ptr<SeedManager> instance_;

	~SeedManager() = default;
	SeedManager(SeedManager&) = delete;
	SeedManager& operator=(SeedManager&) = delete;

	friend struct std::default_delete<SeedManager>;

public:

	// 初期化
	void Initialize();

public: // 外部入出力

	// intで乱数生成
	int GenerateInt(int min, int max);
	// floatで乱数生成
	float GenerateFloat(float min, float max);

	std::mt19937& GetRandomEngine() { return randomEngine_; }

private:

	std::mt19937 randomEngine_;

};

