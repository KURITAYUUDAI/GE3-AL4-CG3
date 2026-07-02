#pragma once
#include <iostream>
#include "DirectXBase.h"
#include <ft2build.h>
#include FT_FREETYPE_H

#include <unordered_map>
#include "freetype/FreeTypeManager.h"

struct FontKey
{
    std::string path;
    uint32_t pixelSize;
    bool operator==(const FontKey&) const = default;
};

struct FontKeyHash
{
    size_t operator()(const FontKey& key) const
    {
        return std::hash<std::string>()(key.path) ^ (std::hash<uint32_t>()(key.pixelSize) << 1);
    }
};

struct GlyphInfo
{
    int width = 0;
    int height = 0;

    int bearingX = 0;
    int bearingY = 0;

    int advance = 0;

    int atlasX = 0;
    int atlasY = 0;

    Vector2 uvMin{};
    Vector2 uvMax{};
};

struct FontAtlas
{
    int width = 0;
    int height = 0;

    // R8_UNORM用
    // 0 = 透明, 255 = 文字
    std::vector<uint8_t> pixels;

    std::unordered_map<char32_t, GlyphInfo> glyphs;

    Microsoft::WRL::ComPtr<ID3D12Resource> textureResource;
    Microsoft::WRL::ComPtr<ID3D12Resource> uploadResource;
    uint32_t srvIndex = 0;
    D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU;
    D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;
};

class FontManager
{
public:

    // シングルトンインスタンスの取得
    static FontManager* GetInstance();
    // 終了
    void Finalize();

    // コンストラクタに渡すための鍵
    class ConstructorKey
    {
    private:
        ConstructorKey() = default;
        friend class FontManager;
    };

    // PassKeyを受け取るコンストラクタ
    explicit FontManager(ConstructorKey){}

private:	// シングルトン化

    static std::unique_ptr<FontManager> instance_;

    ~FontManager() = default;
    FontManager(FontManager&) = delete;
    FontManager& operator=(FontManager&) = delete;

    friend struct std::default_delete<FontManager>;

public:

    // フォントを読み込み、アトラスを構築済みなら参照を返すだけ
    FontAtlas* LoadFont(const std::string& fontPath, uint32_t pixelSize = 32);

    // Sprite側のGetSRVHandleGPU(textureFilePath)相当
    D3D12_GPU_DESCRIPTOR_HANDLE GetSRVHandleGPU(const FontKey& key) const;

private:
    bool BuildAsciiFontAtlas(FT_Face face, FontAtlas& outAtlas, int atlasW, int atlasH);
    bool CreateFontAtlasTexture(FontAtlas& atlas);
    bool CreateFontAtlasSRV(FontAtlas& atlas);

    std::unordered_map<FontKey, std::unique_ptr<FontAtlas>, FontKeyHash> atlases_;
};

