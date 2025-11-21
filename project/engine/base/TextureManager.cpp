#include "TextureManager.h"
#include "DirectXBase.h"
#include "StringUtility.h"
#include "SrvManager.h"

TextureManager* TextureManager::instance = nullptr;

// ImGuiで0番を使用するため、1番から使用
uint32_t TextureManager::kSRVIndexTop = 1;

void TextureManager::Initialize(DirectXBase* dxBase, SrvManager* srvManager)
{
	// SRVの数と同数
	textureDatas_.reserve(SrvManager::kMaxSRVCount);

	dxBase_ = dxBase;

	srvManager_ = srvManager;
}

TextureManager* TextureManager::GetInstance()
{
	if (instance == nullptr)
	{
		instance = new TextureManager;
	}
	return instance;
}

void TextureManager::Finalize()
{
	delete instance;
	instance = nullptr;
}

void TextureManager::LoadTexture(const std::string& filePath)
{
	// 読み込み済みテクスチャを検索
	if (textureDatas_.contains(filePath))
	{
		return;
	}

	// テクスチャ枚数上限チェック
	assert(textureDatas_.size() + kSRVIndexTop < SrvManager::kMaxSRVCount);

	// テクスチャファイルを呼んでプログラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = StringUtility::ConvertString(filePath);
	// DirectX::WIC_FLAGS_FORCE_SRGB : sRGB空間で作られたモノとして読む。
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	// ミップマップの作成　（MipMap : 元画像より小さなテクスチャ群）
	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(hr));
	
	// テクスチャデータを追加して書き込む
	TextureData& textureData = textureDatas_[filePath];

	textureData.metadata = mipImages.GetMetadata();
	textureData.resource = dxBase_->CreateTextureResource(textureData.metadata);

	textureData.srvIndex = srvManager_->Allocate();
	textureData.srvHandleCPU = srvManager_->GetCPUDescriptorHandle(textureData.srvIndex);
	textureData.srvHandleGPU = srvManager_->GetGPUDescriptorHandle(textureData.srvIndex);

	dxBase_->UploadTextureData(textureData.resource.Get(), mipImages);

	// SRV を生成
	srvManager_->CreateSRVforTexture2D(
		textureData.srvIndex,
		textureData.resource.Get(),
		textureData.metadata.format,
		UINT(textureData.metadata.mipLevels)
	);
}

void TextureManager::LoadInstancingTexture(const std::string& filePath, UINT numElements, UINT structureByteStride)
{
	// 読み込み済みテクスチャを検索
	if (instancingTextureDatas_.contains(filePath))
	{
		return;
	}

	// テクスチャ枚数上限チェック
	assert(instancingTextureDatas_.size() + kSRVIndexTop < SrvManager::kMaxSRVCount);

	// テクスチャファイルを呼んでプログラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = StringUtility::ConvertString(filePath);
	// DirectX::WIC_FLAGS_FORCE_SRGB : sRGB空間で作られたモノとして読む。
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	// ミップマップの作成　（MipMap : 元画像より小さなテクスチャ群）
	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(hr));

	// テクスチャデータを追加して書き込む
	TextureData& textureData = instancingTextureDatas_[filePath];

	textureData.metadata = mipImages.GetMetadata();
	textureData.resource = dxBase_->CreateTextureResource(textureData.metadata);

	textureData.srvIndex = srvManager_->Allocate();
	textureData.srvHandleCPU = srvManager_->GetCPUDescriptorHandle(textureData.srvIndex);
	textureData.srvHandleGPU = srvManager_->GetGPUDescriptorHandle(textureData.srvIndex);

	dxBase_->UploadTextureData(textureData.resource.Get(), mipImages);



	// SRV を生成
	srvManager_->CreateSRVforStructuredBuffer(
		textureData.srvIndex,
		textureData.resource.Get(),
		numElements,
		structureByteStride
	);
}

D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetSRVHandleGPU(const std::string& filePath)
{
	//// 範囲外指定違反チェック
	//assert(textureIndex < textureDatas_.size());

	TextureData& textureData = textureDatas_[filePath];

	return textureData.srvHandleGPU;
}

const DirectX::TexMetadata& TextureManager::GetMetaData(const std::string& filePath)
{
	//// 範囲外指定違反チェック
	//assert(textureIndex < textureDatas_.size());

	TextureData& textureData = textureDatas_[filePath];

	return textureData.metadata;
}


