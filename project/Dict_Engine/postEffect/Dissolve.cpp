#include "Dissolve.h"
#include "DirectXBase.h"
#include "PSOManager.h"
#include "SrvManager.h"
#include "TextureManager.h"
#include "Logger.h"
#include <cassert>
#include "../tool/effect/DissolveManager.h"

void Dissolve::Initialize(uint32_t width, uint32_t height)
{
	(void)width;
	(void)height;

    TextureManager::GetInstance()->LoadTexture(maskTextureFilePath_);

	RegisterPSOs();
}

void Dissolve::Finalize()
{

}

std::vector<PostEffect::PassFunc> Dissolve::GetPasses()
{
	return
	{
		[this](D3D12_CPU_DESCRIPTOR_HANDLE destRTV, uint32_t srcSRVIndex){
			Pass(destRTV, srcSRVIndex);
		},
	};
}

std::vector<std::vector<PostEffect::PassBarrier>> Dissolve::GetBarriers()
{
	return {};
}

void Dissolve::Pass(D3D12_CPU_DESCRIPTOR_HANDLE destRTV, uint32_t srcSRVIndex)
{
	auto* cmdList = DirectXBase::GetInstance()->GetCommandList();
	auto* srvManager = SrvManager::GetInstance();

   

	float clearColor[4] = {
	   kClearColor_.x, kClearColor_.y, kClearColor_.z, kClearColor_.w };
	cmdList->ClearRenderTargetView(destRTV, clearColor, 0, nullptr);
	cmdList->OMSetRenderTargets(1, &destRTV, FALSE, nullptr);

	auto psoH = PSOManager::GetInstance()->GetPSOData(
		kPsoName_, PSOManager::BlendMode::None, PSOManager::FillMode::kSolid);
	cmdList->SetGraphicsRootSignature(psoH.rootSignature.Get());
	cmdList->SetPipelineState(psoH.pipelineState.Get());
	srvManager->PreDraw();
	cmdList->SetGraphicsRootDescriptorTable(
		0, srvManager->GetGPUDescriptorHandle(srcSRVIndex));
	DissolveManager::GetInstance()->SetCbufferDissolveResource(1, params_);
    DissolveManager::GetInstance()->SetCbufferMaskTexture(2, maskTextureFilePath_);
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	cmdList->DrawInstanced(3, 1, 0, 0);
}

void Dissolve::RegisterPSOs()
{
    auto makeRootSignature = []() -> ComPtr<ID3D12RootSignature>{
        D3D12_DESCRIPTOR_RANGE srvRange{};
        srvRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
        srvRange.NumDescriptors = 1;
        srvRange.BaseShaderRegister = 0; // t0
        srvRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

        D3D12_DESCRIPTOR_RANGE srvRange1{};
        srvRange1.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
        srvRange1.NumDescriptors = 1;
        srvRange1.BaseShaderRegister = 1; // t1
        srvRange1.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // 追加

        D3D12_ROOT_PARAMETER rootParameters[3]{};
        // [0] t0 : 入力テクスチャ
        rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
        rootParameters[0].DescriptorTable.pDescriptorRanges = &srvRange;
        rootParameters[0].DescriptorTable.NumDescriptorRanges = 1;
        // [1] b0 : BlurParams
        rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
        rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
        rootParameters[1].Descriptor.ShaderRegister = 0; // b0
        rootParameters[1].Descriptor.RegisterSpace = 0;
        // [2] t1 : マスクテクスチャ
        rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
        rootParameters[2].DescriptorTable.NumDescriptorRanges = 1;
        rootParameters[2].DescriptorTable.pDescriptorRanges = &srvRange1;

        // バイリニア + クランプ（線形サンプリング最適化に必須）
        D3D12_STATIC_SAMPLER_DESC sampler =
            PSOManager::GetInstance()->GetDefaultStaticSamplerDesc();
        sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
        sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;

        D3D12_ROOT_SIGNATURE_DESC sigDesc{};
        sigDesc.NumParameters = _countof(rootParameters);
        sigDesc.pParameters = rootParameters;
        sigDesc.NumStaticSamplers = 1;
        sigDesc.pStaticSamplers = &sampler;
        sigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

        ComPtr<ID3DBlob> sigBlob, errBlob;
        HRESULT hr = D3D12SerializeRootSignature(
            &sigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &sigBlob, &errBlob);
        if (FAILED(hr)) {
            Logger::Log(reinterpret_cast<char*>(errBlob->GetBufferPointer()));
            assert(false);
        }

        ComPtr<ID3D12RootSignature> rootSig;
        hr = DirectXBase::GetInstance()->GetDevice()->CreateRootSignature(
            0, sigBlob->GetBufferPointer(), sigBlob->GetBufferSize(),
            IID_PPV_ARGS(&rootSig));
        assert(SUCCEEDED(hr));
        return rootSig;
        };

    auto makeInputLayout = [](){
        return std::vector<D3D12_INPUT_ELEMENT_DESC>{};
        };

    PSOManager::PSOConfig config{};
    config.vertexShaderPath = L"resources/shaders/PostEffect/Fullscreen.VS.hlsl";
    config.pixelShaderPath = L"resources/shaders/PostEffect/Dissolve.PS.hlsl";
    config.rootSignatureGenerator = makeRootSignature;
    config.inputLayoutGenerator = makeInputLayout;
    config.depthEnable = false;
    PSOManager::GetInstance()->RegisterPSOConfig(kPsoName_, config);
}

