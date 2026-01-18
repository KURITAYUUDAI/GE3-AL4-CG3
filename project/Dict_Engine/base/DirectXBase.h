#pragma once
#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <string>
#include <array>

#include <dxcapi.h>
#pragma comment(lib, "dxcompiler.lib")
#include <DirectXMath.h>
using namespace DirectX;

#include "externals/DirectXTex/DirectXTex.h"

// imGuiのinclude
#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"

#include "WindowsAPI.h"
#include "FixFPS.h"

class DirectXBase
{
public:
	// namespace省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:

	// 初期化
	void Initialize(WindowsAPI* winAPI);
	// 更新
	void Update();
	//終了
	void Finalize();

	// 描画前処理
	void PreDraw();

	// 描画後処理
	void PostDraw();

	/// <summary>
	/// シェーダーコンパイル
	/// </summary>
	/// <param name="filePath">CompilerするShaderファイルへのパス</param>
	/// <param name="profile">Compilerに使用するProfile</param>
	/// <returns></returns>
	ComPtr<IDxcBlob> CompileShader(const std::wstring& filePath, const wchar_t* profile);

	// バッファリソース生成
	ComPtr<ID3D12Resource> CreateBufferResource(size_t sizeInBytes);

	
	// テクスチャリソース生成
	ComPtr<ID3D12Resource> CreateTextureResource(const DirectX::TexMetadata& metadata);

	// テクスチャをアップロード
	void UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages);

	



private:	// 各機能の初期化関数

	// デバイスの初期化
	void InitializeDevice();

	// コマンド関連の初期化
	void InitializeCommand();

	// スワップチェーンの初期化
	void InitializeSwapChain(WindowsAPI* winAPI);

	// 深度バッファの生成
	void CreateDepthBuffer();

	// 各種デスクリプタヒープの生成
	void CreateDescriptorHeaps();

	// レンダーターゲットビューの初期化
	void InitializeRenderTargetView();

	// 深度ステンシルビューの初期化
	void InitializeDepthStencilView();

	// フェンスの生成
	void CreateFence();

	// ビューポート矩形の初期化
	void InitializeViewportRect();

	// シザー矩形の初期化
	void InitializeScissorRect();

	// DXCコンパイラの生成
	void CreateDXCCompiler();

	//// ImGuiの初期化
	//void InitializeImGui(WindowsAPI* winAPI);

public: // ゲッター

	// デバイス
	ID3D12Device* GetDevice(){ return device_.Get(); }
	// コマンドリスト
	ID3D12GraphicsCommandList* GetCommandList(){ return commandList_.Get(); }
	// コマンドキュー
	ID3D12CommandQueue* GetCommandQueue(){return commandQueue_.Get(); }

	IDxcUtils* GetDxcUtils(){ return dxcUtils_.Get(); }
	IDxcCompiler3* GetDxcCompiler() { return dxcCompiler_.Get(); }
	IDxcIncludeHandler* GetIncludeHandler() { return includeHandler_.Get(); }

	ID3D12DescriptorHeap* GetRtvDescriptorHeap(){ return rtvDescriptorHeap_.Get(); }
	
	ID3D12DescriptorHeap* GetDsvDescriptorHeap(){ return dsvDescriptorHeap_.Get(); }

	HANDLE GetFenceEvent() { return fenceEvent_; }

	size_t GetSwapChainResourceNum(){ return swapChainResources_.size(); }

public: // その他関数

	/// <summary>
	/// デスクリプタヒープを生成する
	/// </summary>
	ComPtr<ID3D12DescriptorHeap> 
		CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

	D3D12_CPU_DESCRIPTOR_HANDLE GetRTVCPUDescriptorHandle(uint32_t index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetRTVGPUDescriptorHandle(uint32_t index);

	D3D12_CPU_DESCRIPTOR_HANDLE GetDSVCPUDescriptorHandle(uint32_t index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetDSVGPUDescriptorHandle(uint32_t index);

	

private: // 静的関数

	static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(
		const ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize, uint32_t index);

	static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(
		const ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize, uint32_t index);


public:	// 動的変数

	
	

private:

	// WindowsAPI
	WindowsAPI* winAPI_ = nullptr;

	// FixFPS
	std::unique_ptr<FixFPS> fixFPS_ = std::make_unique<FixFPS>();

	// DXCIファクトリー
	ComPtr<IDXGIFactory7> dxgiFactory_ = nullptr;
	// デバイス
	ComPtr<ID3D12Device> device_ = nullptr;

	// コマンドアロケータ
	ComPtr<ID3D12CommandAllocator> commandAllocator_ = nullptr;
	// コマンドリスト
	ComPtr<ID3D12GraphicsCommandList> commandList_ = nullptr;
	// コマンドキュー
	ComPtr<ID3D12CommandQueue> commandQueue_ = nullptr;

	// スワップチェーンのポインタ
	ComPtr<IDXGISwapChain4> swapChain_ = nullptr;

	// Fence生成
	uint64_t fenceValue_ = 0;

	// 深度リソース
	ComPtr<ID3D12Resource> depthResource_ = nullptr;

	// RTV用のヒープでディスクリプタの数は2、RTVはShader内で触るものではないので、ShaderVisibleはfalse
	ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap_ = nullptr;
	uint32_t rtvDescriptorSize_ = 0;

	

	// DSV用のヒープディスクリプタの数は1。DSVはShader内で飾るものではないので、ShaderVisibleはfalse
	ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap_ = nullptr;
	uint32_t dsvDescriptorSize_ = 0;

	// スワップチェーンデスクのバッファカウント
	static const int32_t kSwapChainBufferCount = 2;

	// rtvのフォーマット
	static const DXGI_FORMAT kRtvFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	// スワップチェーン用のリソース
	std::array<ComPtr<ID3D12Resource>, 2> swapChainResources_;

	// RTVハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[2];

	// フェンス
	ComPtr<ID3D12Fence> fence_ = nullptr;

	// ビューポート
	D3D12_VIEWPORT viewport_{};

	// シザー矩形
	D3D12_RECT scissorRect_{};

	// DXCユーティリティ
	ComPtr<IDxcUtils> dxcUtils_ = nullptr;

	// DXCコンパイラの生成
	ComPtr<IDxcCompiler3> dxcCompiler_ = nullptr;

	// インクルードハンドラーの生成
	ComPtr<IDxcIncludeHandler> includeHandler_ = nullptr;
	
	// TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier_{};

	// FenceのSignalを待つためのイベントを作成する
	HANDLE fenceEvent_ = 0;

};


