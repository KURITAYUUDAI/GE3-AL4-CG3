#include "GaussianBlur.h"
#include "DirectXBase.h"
#include "PSOManager.h"
#include "SrvManager.h"
#include "Logger.h"
#include <cassert>

// 定数バッファは256バイトアライン必須（DX12の要件）
// CreateBufferResource()はアライン処理を持たないため直接CreateCommittedResourceを使う
static constexpr size_t kCBVAlignedSize =
(sizeof(GaussianBlur::BlurParams) + 255) & ~255;

void GaussianBlur::Initialize(uint32_t width, uint32_t height)
{
    params_.texelSizeX = 1.0f / static_cast<float>(width);
    params_.texelSizeY = 1.0f / static_cast<float>(height);

    CreateIntermediateBuffer(width, height); // 横パスの書き込み先
    CreateConstantBuffer();
    RegisterPSOs();
}

void GaussianBlur::Draw(ID3D12Resource* srcResource, uint32_t srcSRVIndex,
                         D3D12_CPU_DESCRIPTOR_HANDLE destRTV)
{
    auto* commandList = DirectXBase::GetInstance()->GetCommandList();
    auto* srvManager = SrvManager::GetInstance();

    UpdateConstantBuffer();

    // Pass1：横方向 → intermediateResource_
    TransitionResource(commandList, intermediateResource_.Get(),
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        D3D12_RESOURCE_STATE_RENDER_TARGET);

    float clearColor[4] = {
        kClearColor_.x, kClearColor_.y, kClearColor_.z, kClearColor_.w };
    commandList->ClearRenderTargetView(intermediateRTV_, clearColor, 0, nullptr);
    commandList->OMSetRenderTargets(1, &intermediateRTV_, FALSE, nullptr);

    auto psoH = PSOManager::GetInstance()->GetPSOData(
        kPsoNameH_, PSOManager::BlendMode::None, PSOManager::FillMode::kSolid);
    commandList->SetGraphicsRootSignature(psoH.rootSignature.Get());
    commandList->SetPipelineState(psoH.pipelineState.Get());
    srvManager->PreDraw();
    commandList->SetGraphicsRootDescriptorTable(
        0, srvManager->GetGPUDescriptorHandle(srcSRVIndex));
    commandList->SetGraphicsRootConstantBufferView(
        1, constantBufferResource_->GetGPUVirtualAddress());
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    commandList->DrawInstanced(3, 1, 0, 0);

    TransitionResource(commandList, intermediateResource_.Get(),
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    // Pass2：縦方向 → destRTV（ピンポンバッファ or SwapChain）
    commandList->OMSetRenderTargets(1, &destRTV, FALSE, nullptr);

    auto psoV = PSOManager::GetInstance()->GetPSOData(
        kPsoNameV_, PSOManager::BlendMode::None, PSOManager::FillMode::kSolid);
    commandList->SetGraphicsRootSignature(psoV.rootSignature.Get());
    commandList->SetPipelineState(psoV.pipelineState.Get());
    srvManager->PreDraw();
    commandList->SetGraphicsRootDescriptorTable(
        0, srvManager->GetGPUDescriptorHandle(intermediateSRVIndex_));
    commandList->SetGraphicsRootConstantBufferView(
        1, constantBufferResource_->GetGPUVirtualAddress());
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    commandList->DrawInstanced(3, 1, 0, 0);
}

void GaussianBlur::CreateIntermediateBuffer(uint32_t width, uint32_t height)
{
    auto* dxBase = DirectXBase::GetInstance();

    intermediateResource_ = dxBase->CreateRenderTextureResource(
        width, height, kFormat_,
        Vector4{ kClearColor_.x, kClearColor_.y, kClearColor_.z, kClearColor_.w });

    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
    rtvDesc.Format = kFormat_;
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

    intermediateRTVIndex_ = dxBase->AllocateRTVIndex();
    intermediateRTV_ = dxBase->GetRTVCPUDescriptorHandle(intermediateRTVIndex_);
    dxBase->GetDevice()->CreateRenderTargetView(
        intermediateResource_.Get(), &rtvDesc, intermediateRTV_);

    intermediateSRVIndex_ = SrvManager::GetInstance()->Allocate();

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = kFormat_;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    dxBase->GetDevice()->CreateShaderResourceView(
        intermediateResource_.Get(), &srvDesc,
        SrvManager::GetInstance()->GetCPUDescriptorHandle(intermediateSRVIndex_));
}

void GaussianBlur::CreateConstantBuffer()
{
    auto* device = DirectXBase::GetInstance()->GetDevice();

    D3D12_HEAP_PROPERTIES heapProps{};
    heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

    D3D12_RESOURCE_DESC resDesc{};
    resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resDesc.Width = kCBVAlignedSize; // 256バイトアライン済みのサイズ
    resDesc.Height = 1;
    resDesc.DepthOrArraySize = 1;
    resDesc.MipLevels = 1;
    resDesc.SampleDesc.Count = 1;
    resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    HRESULT hr = device->CreateCommittedResource(
        &heapProps, D3D12_HEAP_FLAG_NONE, &resDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
        IID_PPV_ARGS(&constantBufferResource_));
    assert(SUCCEEDED(hr));

    // 永続マップ（Unmapしない）
    hr = constantBufferResource_->Map(0, nullptr, reinterpret_cast<void**>(&constantBufferMappedData_));
    assert(SUCCEEDED(hr));

    *constantBufferMappedData_ = params_; // 初期値を書き込む
}

void GaussianBlur::UpdateConstantBuffer()
{
    *constantBufferMappedData_ = params_;
}

void GaussianBlur::RegisterPSOs()
{
    auto makeRootSignature = []() -> ComPtr<ID3D12RootSignature>{
        D3D12_DESCRIPTOR_RANGE srvRange{};
        srvRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
        srvRange.NumDescriptors = 1;
        srvRange.BaseShaderRegister = 0; // t0
        srvRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

        D3D12_ROOT_PARAMETER params[2]{};
        // [0] t0 : 入力テクスチャ
        params[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        params[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
        params[0].DescriptorTable.pDescriptorRanges = &srvRange;
        params[0].DescriptorTable.NumDescriptorRanges = 1;
        // [1] b0 : BlurParams
        params[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
        params[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
        params[1].Descriptor.ShaderRegister = 0; // b0

        // バイリニア + クランプ（線形サンプリング最適化に必須）
        D3D12_STATIC_SAMPLER_DESC sampler =
            PSOManager::GetInstance()->GetDefaultStaticSamplerDesc();
        sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
        sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;

        D3D12_ROOT_SIGNATURE_DESC sigDesc{};
        sigDesc.NumParameters = _countof(params);
        sigDesc.pParameters = params;
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

    PSOManager::PSOConfig configH{};
    configH.vertexShaderPath = L"resources/shaders/Fullscreen.VS.hlsl";
    configH.pixelShaderPath = L"resources/shaders/GaussianBlurH.PS.hlsl";
    configH.rootSignatureGenerator = makeRootSignature;
    configH.inputLayoutGenerator = makeInputLayout;
    configH.depthEnable = false;
    PSOManager::GetInstance()->RegisterPSOConfig(kPsoNameH_, configH);

    PSOManager::PSOConfig configV{};
    configV.vertexShaderPath = L"resources/shaders/Fullscreen.VS.hlsl";
    configV.pixelShaderPath = L"resources/shaders/GaussianBlurV.PS.hlsl";
    configV.rootSignatureGenerator = makeRootSignature;
    configV.inputLayoutGenerator = makeInputLayout;
    configV.depthEnable = false;
    PSOManager::GetInstance()->RegisterPSOConfig(kPsoNameV_, configV);
}
