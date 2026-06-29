#pragma once
#include "PostEffect.h"

class RadialBlur : public PostEffect
{
public:
    struct BlurParams
    {
        Vector2 center;
        float bluerWidth;
        Vector2 texelSize;
    };

public:

    void Initialize(uint32_t width, uint32_t height) override;

    void Finalize() override;

public: // 外部入出力

    std::vector<PassFunc> GetPasses() override;
    std::vector<std::vector<PassBarrier>> GetBarriers() override;

    void SetCenter(Vector2 center) { params_.center = center; }
    void SetBlurWidth(float blurWidth) { params_.bluerWidth = blurWidth; }

    Vector2 GetCenter() const { return params_.center; }
    float GetBlurWidth() const { return params_.bluerWidth; }

private:

    // 描画処理
    void Pass(uint32_t srcSRVIndex, D3D12_CPU_DESCRIPTOR_HANDLE destRTV);

    void CreateConstantBuffer();
    void RegisterPSOs();
    void UpdateConstantBuffer();

private:

    // 定数バッファ用リソース
    Microsoft::WRL::ComPtr<ID3D12Resource> constantBufferResource_;
    BlurParams* constantBufferMappedData_ = nullptr;
    BlurParams  params_;

    const std::string kPsoName_ = "RadialBlur";
};

