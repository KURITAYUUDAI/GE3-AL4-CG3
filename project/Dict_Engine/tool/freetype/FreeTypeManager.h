#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H
#include <string>
#include <memory>
#include <unordered_map>

class FreeTypeManager
{
public:

    // シングルトンインスタンスの取得
    static FreeTypeManager* GetInstance();
    // 終了
    void Finalize();

    // コンストラクタに渡すための鍵
    class ConstructorKey
    {
    private:
        ConstructorKey() = default;
        friend class FreeTypeManager;
    };

    // PassKeyを受け取るコンストラクタ
    explicit FreeTypeManager(ConstructorKey){}

private:	// シングルトン化

    static std::unique_ptr<FreeTypeManager> instance_;

    ~FreeTypeManager() = default;
    FreeTypeManager(FreeTypeManager&) = delete;
    FreeTypeManager& operator=(FreeTypeManager&) = delete;

    friend struct std::default_delete<FreeTypeManager>;

public: 
    

    void Initialize();

    // パスからFT_Faceを取得（キャッシュ済みなら使い回す）
    FT_Face GetOrLoadFace(const std::string& fontPath);

private:

    FT_Library library_ = nullptr;
    std::unordered_map<std::string, FT_Face> faces_;
};
