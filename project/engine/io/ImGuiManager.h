#pragma once
#include "WindowsAPI.h"
#include "DirectXBase.h"
#include <memory>

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
	void Initialize(WindowsAPI* winAPI, std::shared_ptr<DirectXBase> dxBase);

	// 前処理
	void Begin();

	// 後処理
	void End();

	// 描画
	void Draw();

private:	// シングルトンインスタンス

	// unique_ptr が delete するために使用する構造体
	struct Deleter 
	{
		void operator()(ImGuiManager* p) const 
		{
			// クラス内部のスコープなので private なデストラクタを呼べる
			delete p;
		}
	};

	// unique_ptr の型定義に Deleter を入れることでdeleteが可能になる
	static std::unique_ptr<ImGuiManager, Deleter> instance_;

	ImGuiManager() = default;
	~ImGuiManager() = default;
	ImGuiManager(ImGuiManager&) = delete;
	ImGuiManager& operator=(ImGuiManager&) = delete;

private:

	std::shared_ptr<DirectXBase> GetDxBaseShared();

private:

	// WindwosAPI
	WindowsAPI* winAPI_ = nullptr;

	// DirectXBase
	std::weak_ptr<DirectXBase> dxBase_;

    ID3D12DescriptorHeap* srvHeap_ = nullptr;
	uint32_t srvIndex_;
    D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU_;
    D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU_;
};

