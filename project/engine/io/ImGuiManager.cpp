#include "ImGuiManager.h"
#include "SrvManager.h"

ImGuiManager* ImGuiManager::instance_ = nullptr;

ImGuiManager* ImGuiManager::GetInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = new ImGuiManager;
	}

	return instance_;
}

void ImGuiManager::Finalize()
{
#ifdef USE_IMGUI

	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

#endif

	delete instance_;
	instance_ = nullptr;



}

void ImGuiManager::Initialize([[maybe_unused]] WindowsAPI* winAPI, [[maybe_unused]] DirectXBase* dxBase)
{
#ifdef USE_IMGUI

	winAPI_ = winAPI;
	dxBase_ = dxBase;

	IMGUI_CHECKVERSION();
	// ImGuiのコンテキストを生成
	ImGui::CreateContext();
	// ImGuiのスタイルを設定
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(winAPI_->GetHwnd()); 

	srvHeap_ = SrvManager::GetInstance()->GetDescriptorHeap();
	srvIndex_ = SrvManager::GetInstance()->Allocate();
	srvHandleCPU_ = SrvManager::GetInstance()->GetCPUDescriptorHandle(srvIndex_);
	srvHandleGPU_ = SrvManager::GetInstance()->GetGPUDescriptorHandle(srvIndex_);

	ImGui_ImplDX12_InitInfo init_info = {};
	init_info.Device = dxBase_->GetDevice();
	init_info.CommandQueue = dxBase_->GetCommandQueue();
	init_info.NumFramesInFlight = static_cast<int>(dxBase_->GetSwapChainResourceNum());
	init_info.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	init_info.LegacySingleSrvCpuDescriptor = srvHandleCPU_;
	init_info.LegacySingleSrvGpuDescriptor = srvHandleGPU_;

	ImGui_ImplDX12_Init(&init_info);

#endif
}

void ImGuiManager::Begin()
{
#ifdef USE_IMGUI
	// Start the Dear ImGui frame

	ImGui_ImplWin32_NewFrame();
	ImGui_ImplDX12_NewFrame();
	ImGui::NewFrame();

	// デモウィンドウ表示
	// ImGui::ShowDemoWindow();
#endif
}

void ImGuiManager::End()
{
#ifdef USE_IMGUI

	ImGui::Render();

#endif
}

void ImGuiManager::Draw()
{
#ifdef USE_IMGUI

	ID3D12GraphicsCommandList* commandList = dxBase_->GetCommandList();

	// デスクリプタヒープの配列をセットするコマンド
	ID3D12DescriptorHeap* ppHeaps[] = { srvHeap_ };
	dxBase_->GetCommandList()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	// 描画コマンドを発光
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);

#endif
}
