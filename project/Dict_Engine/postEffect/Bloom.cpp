#include "Bloom.h"
#include "DirectXBase.h"
#include "PSOManager.h"
#include "SrvManager.h"
#include "Logger.h"
#include <cassert>

void Bloom::Initialize(uint32_t width, uint32_t height)
{
    params_.texelSize.x = 1.0f / static_cast<float>(width);
    params_.texelSize.y = 1.0f / static_cast<float>(height);

    CreateConstantBuffer();
    RegisterPSOs();
}

void Bloom::Finalize()
{
    
}

std::vector<PostEffect::PassFunc> Bloom::GetPasses()
{
    // thisキャプチャでメンバ関数を関数オブジェクトとして返す
    return 
    {
        [this](D3D12_CPU_DESCRIPTOR_HANDLE destRTV, uint32_t srcSRVIndex){
            PassExtract(srcSRVIndex, destRTV);
        },
        [this](D3D12_CPU_DESCRIPTOR_HANDLE destRTV, uint32_t srcSRVIndex){
            PassBlurH(srcSRVIndex, destRTV);
        },
        [this](D3D12_CPU_DESCRIPTOR_HANDLE destRTV, uint32_t srcSRVIndex){
            PassComposite(srcSRVIndex, destRTV);
        }
    };
}

std::vector<std::vector<PostEffect::PassBarrier>> Bloom::GetBarriers()
{
    return {};
}

void Bloom::PassExtract(uint32_t srcSRVIndex, D3D12_CPU_DESCRIPTOR_HANDLE destRTV)
{
    auto* cmdList = DirectXBase::GetInstance()->GetCommandList();
    auto* srvManager = SrvManager::GetInstance();

    mainSceneSRVIndex_ = srcSRVIndex;

    mainSceneSRVIndex_ = srcSRVIndex;
    OutputDebugStringA(("Bloom mainSceneSRVIndex_ = "
        + std::to_string(mainSceneSRVIndex_) + "\n").c_str());

    float clearColor[4] = {
        kClearColor_.x, kClearColor_.y, kClearColor_.z, kClearColor_.w };
    cmdList->ClearRenderTargetView(destRTV, clearColor, 0, nullptr);
    cmdList->OMSetRenderTargets(1, &destRTV, FALSE, nullptr);

    auto psoH = PSOManager::GetInstance()->GetPSOData(
        kPsoNameExtract_, PSOManager::BlendMode::None, PSOManager::FillMode::kSolid);
    cmdList->SetGraphicsRootSignature(psoH.rootSignature.Get());
    cmdList->SetPipelineState(psoH.pipelineState.Get());
    srvManager->PreDraw();
    cmdList->SetGraphicsRootDescriptorTable(
        0, srvManager->GetGPUDescriptorHandle(srcSRVIndex));
    cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cmdList->DrawInstanced(3, 1, 0, 0);
}

void Bloom::PassBlurH(uint32_t srcSRVIndex, D3D12_CPU_DESCRIPTOR_HANDLE destRTV)
{
    auto* cmdList = DirectXBase::GetInstance()->GetCommandList();
    auto* srvManager = SrvManager::GetInstance();

    UpdateConstantBuffer();

    float clearColor[4] = {
        kClearColor_.x, kClearColor_.y, kClearColor_.z, kClearColor_.w };
    cmdList->ClearRenderTargetView(destRTV, clearColor, 0, nullptr);
    cmdList->OMSetRenderTargets(1, &destRTV, FALSE, nullptr);

    auto psoH = PSOManager::GetInstance()->GetPSOData(
        kPsoNameBlurH_, PSOManager::BlendMode::None, PSOManager::FillMode::kSolid);
    cmdList->SetGraphicsRootSignature(psoH.rootSignature.Get());
    cmdList->SetPipelineState(psoH.pipelineState.Get());
    srvManager->PreDraw();
    cmdList->SetGraphicsRootDescriptorTable(
        0, srvManager->GetGPUDescriptorHandle(srcSRVIndex));
    cmdList->SetGraphicsRootConstantBufferView(
        1, constantBufferResource_->GetGPUVirtualAddress());
    cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cmdList->DrawInstanced(3, 1, 0, 0);
}

void Bloom::PassComposite(uint32_t srcSRVIndex, D3D12_CPU_DESCRIPTOR_HANDLE destRTV)
{
    auto* cmdList = DirectXBase::GetInstance()->GetCommandList();
    auto* srvManager = SrvManager::GetInstance();

    float clearColor[4] = {
        kClearColor_.x, kClearColor_.y, kClearColor_.z, kClearColor_.w };
    cmdList->ClearRenderTargetView(destRTV, clearColor, 0, nullptr);
    cmdList->OMSetRenderTargets(1, &destRTV, FALSE, nullptr);

    auto psoH = PSOManager::GetInstance()->GetPSOData(
        kPsoNameComposite_, PSOManager::BlendMode::None, PSOManager::FillMode::kSolid);
    cmdList->SetGraphicsRootSignature(psoH.rootSignature.Get());
    cmdList->SetPipelineState(psoH.pipelineState.Get());
    srvManager->PreDraw();
    cmdList->SetGraphicsRootDescriptorTable(
        0, srvManager->GetGPUDescriptorHandle(srcSRVIndex));
    cmdList->SetGraphicsRootConstantBufferView(
        1, constantBufferResource_->GetGPUVirtualAddress());
    cmdList->SetGraphicsRootDescriptorTable(
        2, srvManager->GetGPUDescriptorHandle(mainSceneSRVIndex_));
    cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cmdList->DrawInstanced(3, 1, 0, 0);
}

void Bloom::CreateConstantBuffer()
{
    // 定数バッファの作成 (BlurParams用)
    auto* dxBase = DirectXBase::GetInstance();
    constantBufferResource_ = dxBase->CreateConstantBufferResource(sizeof(BlurParams)); // ※自作のバッファ生成関数
    constantBufferResource_->Map(0, nullptr, reinterpret_cast<void**>(&constantBufferMappedData_));

    // 定数バッファの初期値を設定
    *constantBufferMappedData_ = params_;
}

void Bloom::RegisterPSOs()
{
    auto makeRootSignatureExtract = []() -> ComPtr<ID3D12RootSignature>{
        D3D12_DESCRIPTOR_RANGE srvRange{};
        srvRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
        srvRange.NumDescriptors = 1;
        srvRange.BaseShaderRegister = 0; // t0
        srvRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

        D3D12_ROOT_PARAMETER params[1]{};
        // [0] t0 : 入力テクスチャ
        params[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        params[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
        params[0].DescriptorTable.pDescriptorRanges = &srvRange;
        params[0].DescriptorTable.NumDescriptorRanges = 1;

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

    PSOManager::PSOConfig configExtract{};
    configExtract.vertexShaderPath = L"resources/shaders/PostEffect/Fullscreen.VS.hlsl";
    configExtract.pixelShaderPath = L"resources/shaders/PostEffect/BloomExtract.PS.hlsl";
    configExtract.rootSignatureGenerator = makeRootSignatureExtract;
    configExtract.inputLayoutGenerator = makeInputLayout;
    configExtract.depthEnable = false;
    PSOManager::GetInstance()->RegisterPSOConfig(kPsoNameExtract_, configExtract);


    auto makeRootSignatureBlur = []() -> ComPtr<ID3D12RootSignature>{
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

    PSOManager::PSOConfig configBlurH{};
    configBlurH.vertexShaderPath = L"resources/shaders/PostEffect/Fullscreen.VS.hlsl";
    configBlurH.pixelShaderPath = L"resources/shaders/PostEffect/GaussianBlurH.PS.hlsl";
    configBlurH.rootSignatureGenerator = makeRootSignatureBlur;
    configBlurH.inputLayoutGenerator = makeInputLayout;
    configBlurH.depthEnable = false;
    PSOManager::GetInstance()->RegisterPSOConfig(kPsoNameBlurH_, configBlurH);


    auto makeRootSignatureComposite = []() -> ComPtr<ID3D12RootSignature>{
        D3D12_DESCRIPTOR_RANGE srvRange{};
        srvRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
        srvRange.NumDescriptors = 1;
        srvRange.BaseShaderRegister = 0; // t0
        srvRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

        D3D12_DESCRIPTOR_RANGE srvRange1{};
        srvRange1.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
        srvRange1.NumDescriptors = 1;
        srvRange1.BaseShaderRegister = 1; // t1

        std::vector<D3D12_ROOT_PARAMETER> rootParameters;
        rootParameters.resize(3);
        // [0] t0 : 入力テクスチャ
        rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
        rootParameters[0].DescriptorTable.pDescriptorRanges = &srvRange;
        rootParameters[0].DescriptorTable.NumDescriptorRanges = 1;
        // [1] b0 : BlurParams
        rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
        rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
        rootParameters[1].Descriptor.ShaderRegister = 0; // b0
        // [2] register(t1) - メイン画面テクスチャ (Compositeパスで必要)
        rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
        rootParameters[2].DescriptorTable.NumDescriptorRanges = 1;
        rootParameters[2].DescriptorTable.pDescriptorRanges = &srvRange1;

        // バイリニア + クランプ（線形サンプリング最適化に必須）
        D3D12_STATIC_SAMPLER_DESC sampler =
            PSOManager::GetInstance()->GetDefaultStaticSamplerDesc();
        sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
        sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;

        D3D12_ROOT_SIGNATURE_DESC signatureDesc{};
        signatureDesc.NumParameters = (UINT)rootParameters.size();
        signatureDesc.pParameters = rootParameters.data();
        signatureDesc.NumStaticSamplers = 1;
        signatureDesc.pStaticSamplers = &sampler;
        signatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

        ComPtr<ID3DBlob> sigBlob, errBlob;
        HRESULT hr = D3D12SerializeRootSignature(
            &signatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &sigBlob, &errBlob);
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

    PSOManager::PSOConfig configComposite{};
    configComposite.vertexShaderPath = L"resources/shaders/PostEffect/Fullscreen.VS.hlsl";
    configComposite.pixelShaderPath = L"resources/shaders/PostEffect/BloomComposite.PS.hlsl";
    configComposite.rootSignatureGenerator = makeRootSignatureComposite;
    configComposite.inputLayoutGenerator = makeInputLayout;
    configComposite.depthEnable = false;
    PSOManager::GetInstance()->RegisterPSOConfig(kPsoNameComposite_, configComposite);
    
}

void Bloom::UpdateConstantBuffer()
{
    *constantBufferMappedData_ = params_;
}



