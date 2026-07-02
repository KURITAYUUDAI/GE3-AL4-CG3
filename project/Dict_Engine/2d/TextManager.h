#pragma once
#include "DirectXBase.h"
#include "PSOManager.h"

#include <memory>

class TextManager
{
public:

	// シングルトンインスタンスの取得
	static TextManager* GetInstance();
	// 終了
	void Finalize();

	// コンストラクタに渡すための鍵
	class ConstructorKey
	{
	private:
		ConstructorKey() = default;
		friend class TextManager;
	};

	// PassKeyを受け取るコンストラクタ
	explicit TextManager(ConstructorKey){}

private:	// シングルトン化

	static std::unique_ptr<TextManager> instance_;

	~TextManager() = default;
	TextManager(TextManager&) = delete;
	TextManager& operator=(TextManager&) = delete;

	friend struct std::default_delete<TextManager>;

public:

    void Initialize();

	void RegisterPSO();

public: // 外部入力＆出力

    const std::string& GetDefaultPsoName() const { return psoName_; }
    const PSOManager::BlendMode& GetDefaultBlendMode() const { return blendMode_; }

private:

    std::string psoName_ = "Font";
    PSOManager::BlendMode blendMode_ = PSOManager::BlendMode::Normal;
    PSOManager::FillMode fillMode_ = PSOManager::FillMode::kSolid;
};

