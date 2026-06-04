#include "PostEffectManager.h"
#include "DirectXBase.h"
#include "PSOManager.h"
#include "SrvManager.h"
#include "OffscreenRender.h"
#include "Logger.h"
#include <cassert>

std::unique_ptr<PostEffectManager> PostEffectManager::instance_ = nullptr;

PostEffectManager* PostEffectManager::GetInstance()
{
    if (instance_ == nullptr)
    {
        instance_ = std::make_unique<PostEffectManager>(ConstructorKey());
    }
    return instance_.get();
}

void PostEffectManager::Finalize()
{
    chain_.clear();
    factories_.clear();
    instance_.reset();
}

// ---------------------------------------------------------------
//  初期化
// ---------------------------------------------------------------
void PostEffectManager::Initialize(uint32_t width, uint32_t height)
{
    width_ = width;
    height_ = height;

    // エフェクトが0個のときのフォールバック用PSOを登録
    RegisterPassthroughPSO();

    isReady_ = true;
}

// ---------------------------------------------------------------
//  ファクトリ登録
// ---------------------------------------------------------------
void PostEffectManager::RegisterFactory(const std::string& name, FactoryFunc factory)
{
    assert(factories_.find(name) == factories_.end()
        && "PostEffectManager: 同じ名前のエフェクトが既に登録されています");

    factories_[name] = std::move(factory);
}

// ---------------------------------------------------------------
//  エフェクトチェーンの操作
// ---------------------------------------------------------------
void PostEffectManager::Clear()
{
	for (auto& chainEffect : chain_)
	{
		chainEffect->Finalize();
	}
    chain_.clear();
}

void PostEffectManager::Add(const std::string& name)
{
    auto it = factories_.find(name);
    assert(it != factories_.end()
        && "PostEffectManager: 未登録のエフェクト名です。RegisterFactory()を先に呼んでください");

    // ファクトリで生成 → 初期化 → チェーンに追加
    auto effect = it->second();
    effect->SetName(name);
    effect->Initialize(width_, height_);
    chain_.push_back(std::move(effect));
}

void PostEffectManager::Remove(uint32_t index)
{
    assert(index < chain_.size() && "PostEffectManager: Remove インデックスが範囲外です");
    chain_.erase(chain_.begin() + index);
}

void PostEffectManager::Swap(uint32_t indexA, uint32_t indexB)
{
    assert(indexA < chain_.size() && "PostEffectManager: Swap indexA が範囲外です");
    assert(indexB < chain_.size() && "PostEffectManager: Swap indexB が範囲外です");
    std::swap(chain_[indexA], chain_[indexB]);
}

// ---------------------------------------------------------------
//  毎フレーム描画
//
//  チェーンの流れ:
//    エフェクト0個 → DrawPassthrough（シーンRTをそのままSwapChainへ）
//    エフェクト1個 → effect[0].DrawFinal()
//    エフェクトN個 → effect[0].Draw() → ... → effect[N-1].DrawFinal()
// ---------------------------------------------------------------
void PostEffectManager::Draw(ID3D12Resource* srcResource, uint32_t srcSRVIndex)
{
    assert(isReady_);

    if (chain_.empty())
    {
        DrawPassthrough(srcResource, srcSRVIndex);
        return;
    }

    auto* cmdList = DirectXBase::GetInstance()->GetCommandList();
    auto* srvManager = SrvManager::GetInstance();

    // 現在の入力
    ID3D12Resource* currentSrc = srcResource;
    uint32_t        currentSRVIdx = srcSRVIndex;

    // ピンポンバッファのどちらに書くか（0 or 1）
    int dstPing = 0;

    for (size_t i = 0; i < chain_.size(); ++i)
    {
        bool isLast = (i == chain_.size() - 1);

        if (isLast)
        {
            // 最後のエフェクトはSwapChainへ直接書き込む
            chain_[i]->Draw(currentSrc, currentSRVIdx,
                DirectXBase::GetInstance()->GetBackBufferRTVHandle());
        } else
        {
            // 中間エフェクトはピンポンバッファへ書き込む
            PostEffect::TransitionResource(cmdList, pingPongRT_[dstPing].Get(),
                D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
                D3D12_RESOURCE_STATE_RENDER_TARGET);

            chain_[i]->Draw(currentSrc, currentSRVIdx, pingPongRTV_[dstPing]);

            PostEffect::TransitionResource(cmdList, pingPongRT_[dstPing].Get(),
                D3D12_RESOURCE_STATE_RENDER_TARGET,
                D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

            // 次のエフェクトの入力を今書いたピンポンバッファに切り替え
            currentSrc = pingPongRT_[dstPing].Get();
            currentSRVIdx = pingPongSRVIndex_[dstPing];
            dstPing = 1 - dstPing; // 0→1→0→1 と交互に切り替え
        }
    }
}

// ---------------------------------------------------------------
//  情報取得
// ---------------------------------------------------------------
const std::string& PostEffectManager::GetEffectName(uint32_t index) const
{
    assert(index < chain_.size() && "PostEffectManager: GetEffectName インデックスが範囲外です");
    return chain_[index]->GetName();
}

// ---------------------------------------------------------------
//  フォールバック描画（エフェクト0個時）
//  シーンRTをそのままSwapChainに転写する
//  ※ 既存の OffscreenRender::Draw() と同等の処理
// ---------------------------------------------------------------
void PostEffectManager::DrawPassthrough(ID3D12Resource* srcResource, uint32_t srcSRVIndex)
{
    auto* cmdList = DirectXBase::GetInstance()->GetCommandList();
    auto* srvManager = SrvManager::GetInstance();

    auto psoSet = PSOManager::GetInstance()->GetPSOData(
        kPsoNamePassthrough_,
        PSOManager::BlendMode::None,
        PSOManager::FillMode::kSolid);

    cmdList->SetGraphicsRootSignature(psoSet.rootSignature.Get());
    cmdList->SetPipelineState(psoSet.pipelineState.Get());
    srvManager->PreDraw();
    cmdList->SetGraphicsRootDescriptorTable(
        0, srvManager->GetGPUDescriptorHandle(srcSRVIndex));
    cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cmdList->DrawInstanced(3, 1, 0, 0);
}

// ---------------------------------------------------------------
//  フォールバック用PSO登録（既存の"Fullscreen"PSOと同じ構成）
// ---------------------------------------------------------------
void PostEffectManager::RegisterPassthroughPSO()
{
    PSOManager::PSOConfig config{};
    config.vertexShaderPath = L"resources/shaders/Fullscreen.VS.hlsl";
    config.pixelShaderPath = L"resources/shaders/Fullscreen.PS.hlsl";

    config.rootSignatureGenerator = []() -> ComPtr<ID3D12RootSignature>{
        D3D12_DESCRIPTOR_RANGE srvRange{};
        srvRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
        srvRange.NumDescriptors = 1;
        srvRange.BaseShaderRegister = 0;
        srvRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

        D3D12_ROOT_PARAMETER param{};
        param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        param.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
        param.DescriptorTable.pDescriptorRanges = &srvRange;
        param.DescriptorTable.NumDescriptorRanges = 1;

        D3D12_STATIC_SAMPLER_DESC sampler =
            PSOManager::GetInstance()->GetDefaultStaticSamplerDesc();

        D3D12_ROOT_SIGNATURE_DESC sigDesc{};
        sigDesc.NumParameters = 1;
        sigDesc.pParameters = &param;
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

    config.inputLayoutGenerator = [](){
        return std::vector<D3D12_INPUT_ELEMENT_DESC>{};
        };

    config.depthEnable = false;

    PSOManager::GetInstance()->RegisterPSOConfig(kPsoNamePassthrough_, config);
}
