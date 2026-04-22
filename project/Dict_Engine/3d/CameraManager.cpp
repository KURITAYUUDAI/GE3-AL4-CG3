#include "CameraManager.h"
#include "Camera.h"

std::unique_ptr<CameraManager> CameraManager::instance_ = nullptr;

CameraManager* CameraManager::GetInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = std::make_unique<CameraManager>(ConstructorKey());
	}
	return instance_.get();
}

void CameraManager::Finalize()
{
	instance_.reset();
}

void CameraManager::Initialize()
{
	CreateCameraResource();
}

void CameraManager::Update()
{
	if (activeCamera_)
	{
		// カメラの更新
		activeCamera_->Update();
		// カメラのワールド座標をGPU用構造体に転送
		cameraData_->worldPosition = activeCamera_->GetTranslate();
	}
}

void CameraManager::SetCbufferCameraResource(UINT RootParameterIndex)
{
	// カメラ用のCBufferをバインド
	DirectXBase::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(RootParameterIndex, cameraResource_->GetGPUVirtualAddress());
}

void CameraManager::AddCamera(const std::string& name, Camera* camera)
{
	cameras_[name] = camera;

	if (!activeCamera_)
	{
		activeCamera_ = camera;
	}
}

void CameraManager::SetActiveCamera(const std::string& name)
{
	auto it = cameras_.find(name);
	if (it != cameras_.end())
	{
		activeCamera_ = it->second;
	}
}

void CameraManager::CreateCameraResource()
{
	// カメラ用リソースを作成する
	cameraResource_ = DirectXBase::GetInstance()->CreateBufferResource(sizeof(CameraForGPU));
	// CameraResourceにデータを書き込むためのアドレスを取得してCameraDataに割り当てる
	cameraResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraData_));

	// カメラデータの初期値を書き込む
	cameraData_->worldPosition = { 0.0f, 0.0f, 0.0f };

}

