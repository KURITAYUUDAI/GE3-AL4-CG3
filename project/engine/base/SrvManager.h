#pragma once
#include "DirectXBase.h"

// SRV管理
class SrvManager
{
public:
	// namespace省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
  
	// シングルトンインスタンスの取得
	static SrvManager* GetInstance();
	// 終了
	void Finalize();

public:


	// 初期化
	void Initialize(DirectXBase* dxBase);

	void PreDraw();

	void SetGraphicsRootDescriptorTable(UINT RootParameterIndex, uint32_t srvIndex);

	uint32_t Allocate();

	// SRV生成（テクスチャ用）
	void CreateSRVforTexture2D(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT Format, UINT MipLevels);
	// SRV生成（Structured Buffer用）
	void CreateSRVforStructuredBuffer(uint32_t srvIndex, ID3D12Resource* pResource, 
		UINT numElements, UINT structureByteStride);

public: // 外部入出力

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(const uint32_t index);

	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(const uint32_t index);

public:

	// 最大SRV数（最大テクスチャ枚数）
	static const uint32_t kMaxSRVCount;

private: 	// シングルトンインスタンス

	static SrvManager* instance;

	SrvManager() = default;
	~SrvManager() = default;
	SrvManager(SrvManager&) = delete;
	SrvManager& operator=(SrvManager&) = delete;


private:
	
	// DirectXBase
	DirectXBase* dxBase_ = nullptr; 

	
	// SRV用のヒープでディスクリプタの数は128、SRVはShader内で触るものなので、ShaderVisibleはtrue
	ComPtr<ID3D12DescriptorHeap> descriptorHeap_ = nullptr;
	uint32_t descriptorSize_ = 0;

	// 次に使用するSRVインデックス
	uint32_t useIndex_ = 0;


};

