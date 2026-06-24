#pragma once
#include "PostEffect.h"

class Random : public PostEffect
{
public:
    void Initialize(uint32_t width, uint32_t height) override;

    void Finalize() override;


public: // 外部入出力

    std::vector<PassFunc> GetPasses() override;
    std::vector<std::vector<PassBarrier>> GetBarriers() override;

    void SetTime(LONGLONG time) { time_ = time; }

private:

    void Pass(uint32_t srcSRVIndex, D3D12_CPU_DESCRIPTOR_HANDLE destRTV);
    
    void CreateConstantBuffer();
    void RegisterPSOs();
    void UpdateConstantBuffer();

private:

    ComPtr<ID3D12Resource> constantBufferResource_;
    LONGLONG* constantBufferMappedData_ = nullptr;
    LONGLONG  time_;

    const std::string kPsoName_ = "Random";
};

