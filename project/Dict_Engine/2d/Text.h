#pragma once
#include "TextManager.h"
#include "FontManager.h"
#include "WorldTransform.h"

class Text
{
public:

    struct VertexData
    {
        Vector4 position;
        Vector2 texcoord;
        Vector3 normal;
    };

    struct Material
    {
        Vector4 color;
    };

    struct TransformationMatrix
    {
        Matrix4x4 WVP;
        Matrix4x4 World;
    };

public:

    void Initialize(const std::string& fontPath, uint32_t pixelSize, const std::u32string& text);
    void Update();
    void Draw();
    void Finalize();

public: // 外部入力＆出力

    void SetText(const std::u32string& text); // 変更時のみレイアウト再構築
    
    void SetPosition(const Vector2& position) { position_ = position; }
    void SetColor(const Vector4& color) { materialData_->color = color; }

    const Vector2 GetPosition(){ return position_; }
    const Vector4& GetColor() const { return materialData_->color; }

private:

    void CreateVertexResource(); // kMaxTextLength分を確保
    void CreateMaterialResource();
    void RebuildMesh(); // text_からvertexData_を再構築、indexCount_を更新

    static constexpr uint32_t kMaxTextLength = 256;

private:

    Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
    VertexData* vertexData_ = nullptr;
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

    Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;
    uint32_t* indexData_ = nullptr;
    D3D12_INDEX_BUFFER_VIEW indexBufferView_{};

    uint32_t currentIndexCount_ = 0; // 実際の文字数×6

    Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
    Material* materialData_ = nullptr;


    TextManager* textManager_ = nullptr;
    FontAtlas* fontAtlas_ = nullptr; // FontManagerから取得した参照
    FontKey fontKey_{};

    WorldTransform worldTransform_;
    Vector2 position_ = { 0.0f, 0.0f };
    float lineHeight_ = 32.0f;

    std::u32string text_;
};

