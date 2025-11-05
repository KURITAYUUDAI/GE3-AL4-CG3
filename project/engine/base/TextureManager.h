#pragma once
#include <Windows.h>
#include <string>

#include <d3d12.h>

#include "externals/DirectXTex/DirectXTex.h"
#include <wrl.h>

class DirectXBase;

class TextureManager
{
public:

	// シングルトンインスタンスの取得
	static TextureManager* GetInstance();
	// 終了
	void Finalize();

	/// <summary>
	/// テクスチャファイルの読み込み
	/// </summary>
	/// <param name="filePath">テクスチャファイルのパス</param>
	/// <returns>画像イメージデータ</returns>
	void LoadTexture(const std::string& filePath);

	// SRVインデックスの開始番号
	uint32_t GetTextureIndexByFilePath(const std::string& filePath);

public:	// 外部入出力

	// ゲッター
	D3D12_GPU_DESCRIPTOR_HANDLE GetSRVHandleGPU(uint32_t textureIndex);
	const DirectX::TexMetadata& GetMetaData(uint32_t textureIndex);

	// セッター
	void SetDxBase(DirectXBase* dxBase){ dxBase_ = dxBase; }

public:	// 動的変数

	// SRVインデックスの開始番号
	static uint32_t kSRVIndexTop;


private:

	// テクスチャ1枚分のデータ
	struct TextureData
	{
		std::string filePath;
		DirectX::TexMetadata metadata;
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU;
		D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;
	};

private:	// シングルトン化

	static TextureManager* instance;

	TextureManager() = default;
	~TextureManager() = default;
	TextureManager(TextureManager&) = delete;
	TextureManager& operator=(TextureManager&) = delete;

private:	// 静的関数

	// 初期化
	void Initialize(DirectXBase* dxBase);

private:	// 静的変数

	// テクスチャデータ
	std::vector<TextureData> textureDatas_;

	DirectXBase* dxBase_;

	

};