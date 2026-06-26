#pragma once
#include "PostEffect.h"

class Darken : public PostEffect
{
public:

    struct DarkenParam
    {
        float intensity = 0.5f;
        float padding[3]{};
        Vector4 darkColor = { 0.0f, 0.0f, 0.0f, 0.0f };
    };

public:
    void Initialize(uint32_t width, uint32_t height) override;

    void Finalize() override;


public: // 外部入出力

    std::vector<PassFunc> GetPasses() override;
    std::vector<std::vector<PassBarrier>> GetBarriers() override;

    void SetIntensity(const float& intensity) { param_.intensity = intensity; }
    void SetDarkColor(const Vector4& darkColor) { param_.darkColor = darkColor; }

    const float& GetIntensity() const { return param_.intensity; }
    const Vector4& GetDarkColor() const { return param_.darkColor; }

private:

    void Pass(uint32_t srcSRVIndex, D3D12_CPU_DESCRIPTOR_HANDLE destRTV);

    void CreateConstantBuffer();
    void RegisterPSOs();
    void UpdateConstantBuffer();

private:

    ComPtr<ID3D12Resource> constantBufferResource_;
    DarkenParam* constantBufferMappedData_ = nullptr;
    DarkenParam param_;

    const std::string kPsoName_ = "Darken";
};

