#pragma once
#include "PostEffect.h"

class Outline : public PostEffect
{
public:
    void Initialize(uint32_t width, uint32_t height) override;

    void Finalize() override;


public: // 外部入出力

    std::vector<PassFunc> GetPasses(uint32_t srcSRVIndex) override;

    std::vector<std::vector<PassBarrier>> GetBarriers() override;

private:

    void Pass(D3D12_CPU_DESCRIPTOR_HANDLE destRTV, uint32_t srcSRVIndex);

    void RegisterPSOs();

	void CreateDepthStencilResource();

private:

	ID3D12Resource* depthStencilResource_ = nullptr;
	D3D12_CPU_DESCRIPTOR_HANDLE depthStencilDescriptorHandle_ = {};
	uint32_t depthStencilSrvIndex_ = 0;

	PassBarrier depthStencilBarrier_ = {};

	const std::string kPsoName_ = "Outline";
};

