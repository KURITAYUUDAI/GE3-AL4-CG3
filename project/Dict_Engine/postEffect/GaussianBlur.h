#pragma once
#include "PostEffect.h"

class GaussianBlur : public PostEffect
{
public:
    // GPUに渡す定数バッファ構造体
    // HLSL側の cbuffer BlurParams と順序・型を一致させること
    struct BlurParams
    {
        float sigma = 5.0f;  // ぼかし強度（標準偏差）
        int   kernelRadius = 16;    // カーネル半径（サイズ = 2*radius+1）
        float texelSizeX = 0.0f; // 1テクセル分のU幅（Initialize時に自動設定）
        float texelSizeY = 0.0f; // 1テクセル分のV幅（Initialize時に自動設定）
    };

public:
    void Initialize(uint32_t width, uint32_t height) override;

	void Finalize() override;


public: // 外部入出力

    std::vector<PassFunc> GetPasses(uint32_t srcSRVIndex) override;
    std::vector<std::vector<PassBarrier>> GetBarriers() override;

    void SetSigma(float sigma) { params_.sigma = sigma; }
	void SetKernelRadius(int kernelRadius) { params_.kernelRadius = kernelRadius; }

	float GetSigma() const { return params_.sigma; }
	int GetKernelRadius() const { return params_.kernelRadius; }


private:
    
    // 横パス・縦パスそれぞれの描画処理
    void PassH(uint32_t srcSRVIndex, D3D12_CPU_DESCRIPTOR_HANDLE destRTV);
    void PassV(uint32_t srcSRVIndex, D3D12_CPU_DESCRIPTOR_HANDLE destRTV);

    void CreateConstantBuffer();
    void RegisterPSOs();
    void UpdateConstantBuffer();

private:

    ComPtr<ID3D12Resource> constantBufferResource_;
    BlurParams* constantBufferMappedData_ = nullptr;
    BlurParams  params_;

    const std::string kPsoNameH_ = "GaussianBlurH";
    const std::string kPsoNameV_ = "GaussianBlurV";
};
