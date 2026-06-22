#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include "DissolveUtility.h"

class DissolveManager
{
public:
	// シングルトンインスタンスの取得
	static DissolveManager* GetInstance();
	// 終了
	void Finalize();

	// コンストラクタに渡すための鍵
	class ConstructorKey
	{
	private:
		ConstructorKey() = default;
		friend class DissolveManager;
	};

	// PassKeyを受け取るコンストラクタ
	explicit DissolveManager(ConstructorKey){}

private: 	// シングルトンインスタンス

	static std::unique_ptr<DissolveManager> instance_;

	~DissolveManager() = default;
	DissolveManager(DissolveManager&) = delete;
	DissolveManager& operator=(DissolveManager&) = delete;

	friend struct std::default_delete<DissolveManager>;

public:
	void Initialize();

	void BeginFrame();

	void SetCbufferDissolveResource(UINT RootParameterIndex, const DissolveParams& dissolveParams);

	void SetCbufferMaskTexture(UINT RootParameterIndex, const uint32_t& index);

	void SetCbufferMaskTexture(UINT RootParameterIndex, const std::string& textureFilePath);

public:

	 const uint32_t& AddMaskTexture(const std::string& textureFilePath);

private:

	// カメラ用リソースを作成
	void CreateDissolveResource();

private:

	static const size_t kMaxObjects = 512; // 1フレームでディゾルブできる最大数
	// 256バイトの倍数にアライメントされたサイズを計算（DX12の定数バッファの必須ルール）
	static const size_t kConstBufferSize = (sizeof(DissolveParams) + 255) & ~255;

	uint32_t currentInstanceIndex_ = 0; // 今何個目のオブジェクトを処理しているか

	// ディゾルブ用リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> dissolveResource_ = nullptr;
	// バッファリソース内のデータを指すポインタ
	uint8_t* dissolveDataBegin_ = nullptr;

	std::vector<std::string> maskTextureFilePasses_;
	uint32_t nextMaskTextureIndex_ = -1;

};

