#include "DissolveManager.h"
#include "DirectXBase.h"
#include "TextureManager.h"
#include "Logger.h"


std::unique_ptr<DissolveManager> DissolveManager::instance_ = nullptr;

DissolveManager* DissolveManager::GetInstance()
{
    if (instance_ == nullptr)
    {
        instance_ = std::make_unique<DissolveManager>(ConstructorKey());
    }
    return instance_.get();
}

void DissolveManager::Finalize()
{
    maskTextureFilePasses_.clear();
    instance_.reset();
}

void DissolveManager::Initialize()
{
    CreateDissolveResource();

    TextureManager::GetInstance()->LoadTexture("noise0.png");
    TextureManager::GetInstance()->LoadTexture("noise1.png");

    maskTextureFilePasses_.push_back("noise0.png");
    nextMaskTextureIndex_++;
    maskTextureFilePasses_.push_back("noise1.png");
    nextMaskTextureIndex_++;
}

void DissolveManager::BeginFrame()
{
    // 毎フレームの描画開始前に0に戻す
    currentInstanceIndex_ = 0;
}

void DissolveManager::SetCbufferDissolveResource(UINT RootParameterIndex, const DissolveParams & dissolveParams)
{
    if (currentInstanceIndex_ >= kMaxObjects) {
        Logger::Log("Dissolve limit exceeded!");
        assert(false);
    }

    // 現在のオブジェクト用のメモリ番地を計算して書き込み
    uint8_t* targetAddress = dissolveDataBegin_ + (kConstBufferSize * currentInstanceIndex_);
    memcpy(targetAddress, &dissolveParams, sizeof(DissolveParams));

    // GPU側の仮想アドレスもその分だけずらしてバインド
    D3D12_GPU_VIRTUAL_ADDRESS gpuAddress = dissolveResource_->GetGPUVirtualAddress() + (kConstBufferSize * currentInstanceIndex_);

    DirectXBase::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(
        RootParameterIndex, gpuAddress);

    currentInstanceIndex_++;
}

void DissolveManager::SetCbufferMaskTexture(UINT RootParameterIndex, const uint32_t& index)
{
    if (index >= maskTextureFilePasses_.size())
    {
        Logger::Log("This index is not registered in maskTexture.");
        assert(false);
    }

    DirectXBase::GetInstance()->GetCommandList()->SetGraphicsRootDescriptorTable(
       RootParameterIndex, TextureManager::GetInstance()->GetSRVHandleGPU(maskTextureFilePasses_[index]));
}

void DissolveManager::SetCbufferMaskTexture(UINT RootParameterIndex, const std::string& textureFilePath)
{
    DirectXBase::GetInstance()->GetCommandList()->SetGraphicsRootDescriptorTable(
      RootParameterIndex, TextureManager::GetInstance()->GetSRVHandleGPU(textureFilePath));
}

const uint32_t& DissolveManager::AddMaskTexture(const std::string& textureFilePath)
{
    maskTextureFilePasses_.push_back(textureFilePath);
    nextMaskTextureIndex_++;
    return nextMaskTextureIndex_;
}

void DissolveManager::CreateDissolveResource()
{
    // 定数バッファの作成
    auto* dxBase = DirectXBase::GetInstance();
    dissolveResource_ = dxBase->CreateConstantBufferResource(kConstBufferSize * kMaxObjects);
    dissolveResource_->Map(0, nullptr, reinterpret_cast<void**>(&dissolveDataBegin_));
}
