#pragma once
#include "PostEffect.h"

class Bloom : public PostEffect
{
public:
    struct BlurParams
    {
        float sigma = 5.0f;
        int kernelRadius = 16;
        Vector2 texelSize;
    };

public:

    void Initialize(uint32_t width, uint32_t height) override;

    void Finalize() override;

public: // 外部入出力

    std::vector<PassFunc> GetPasses() override;
    std::vector<std::vector<PassBarrier>> GetBarriers() override;

    void SetSigma(float sigma) { params_.sigma = sigma; }
    void SetKernelRadius(int kernelRadius) { params_.kernelRadius = kernelRadius; }

    float GetSigma() const { return params_.sigma; }
    int GetKernelRadius() const { return params_.kernelRadius; }
    
private:

    // 横パス・縦パスそれぞれの描画処理
    void PassExtract(uint32_t srcSRVIndex, D3D12_CPU_DESCRIPTOR_HANDLE destRTV);
    void PassBlurH(uint32_t srcSRVIndex, D3D12_CPU_DESCRIPTOR_HANDLE destRTV);
    void PassComposite(uint32_t srcSRVIndex, D3D12_CPU_DESCRIPTOR_HANDLE destRTV);

    void CreateConstantBuffer();
    void RegisterPSOs();
    void UpdateConstantBuffer();

private:

   

    // 定数バッファ用リソース
    Microsoft::WRL::ComPtr<ID3D12Resource> constantBufferResource_;
    BlurParams* constantBufferMappedData_ = nullptr;
    BlurParams  params_;

    uint32_t mainSceneSRVIndex_ = 0;

    const std::string kPsoNameBlurH_ = "BloomBlurH";
    const std::string kPsoNameExtract_ = "BloomExtract";
    const std::string kPsoNameComposite_ = "BloomComposite";

};

