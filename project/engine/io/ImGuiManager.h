#pragma once
#include "WindowsAPI.h"
#include "DirectXBase.h"

#ifdef USE_IMGUI
#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"
#endif

class ImGuiManager
{
public:

	// シングルトンインスタンスの取得
	static ImGuiManager* GetInstance();
	// 終了
	void Finalize();

public:

	// 初期化
	void Initialize(WindowsAPI* winAPI, DirectXBase* dxBase);

	// 前処理
	void Begin();

	// 後処理
	void End();

	// 描画
	void Draw();

private:	// シングルトンインスタンス

	static ImGuiManager* instance_;

	ImGuiManager() = default;
	~ImGuiManager() = default;
	ImGuiManager(ImGuiManager&) = delete;
	ImGuiManager& operator=(ImGuiManager&) = delete;

private:

	// WindwosAPI
	WindowsAPI* winAPI_ = nullptr;

	// DirectXBase
	DirectXBase* dxBase_ = nullptr;

    ID3D12DescriptorHeap* srvHeap_ = nullptr;
	uint32_t srvIndex_;
    D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU_;
    D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU_;
};

