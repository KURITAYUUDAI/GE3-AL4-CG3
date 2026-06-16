#include "CameraManager.h"
//#include "Camera.h"
#include "ICameraController.h"

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
	if (mainCamera_)
	{
		mainCamera_->Finalize();
	}
	instance_.reset();
}

void CameraManager::Initialize()
{
	mainCamera_ = std::make_unique<Camera>();
	mainCamera_->Initialize();

	/*DefaultCameraController* defaultController = new DefaultCameraController();
	defaultController->Initialize();
	AddCameraController("Default", defaultController);
	SetActiveCameraController("Default");*/

	CreateCameraResource();
	CreateProjectionInverseResource();
}

void CameraManager::Update(const float& deltaTime)
{
	if (activeCameraController_ && mainCamera_)
	{
	#ifdef _DEBUG

		ImGui::Begin("CameraSetting");
		Vector3 cameraRotate = activeCameraController_->GetWorldTransform()->GetRotate();
		Vector3 cameraTranslate = activeCameraController_->GetWorldTransform()->translate_;
		if (ImGui::DragFloat3("CameraRotate", &cameraRotate.x, (1.0f / 180.0f) * pi))
		{
			activeCameraController_->GetWorldTransform()->SetRotate(cameraRotate);
		}
		if (ImGui::DragFloat3("CameraTranslate", &cameraTranslate.x, 0.1f))
		{
			activeCameraController_->GetWorldTransform()->translate_ = cameraTranslate;
		}

		ImGui::End();

	#endif

		activeCameraController_->Update(mainCamera_.get(), deltaTime);

		mainCamera_->Update();

		// カメラのワールド座標をGPU用構造体に転送
		cameraData_->worldPosition = mainCamera_->GetTranslate();

		// カメラの逆行列をGPU用構造体に転送
		projectionInverseData_->projectionInverse = Inverse(mainCamera_->GetProjectionMatrix());
	}

	//if (activeCamera_)
	//{
	//	// カメラの更新
	//	activeCamera_->Update();
	//	// カメラのワールド座標をGPU用構造体に転送
	//	cameraData_->worldPosition = activeCamera_->GetTranslate();
	//	// カメラの逆行列をGPU用構造体に転送
	//	projectionInverseData_->projectionInverse = Inverse(activeCamera_->GetProjectionMatrix());
	//}
}

void CameraManager::DrawDebugUI()
{
	if (activeCameraController_ && mainCamera_)
	{
		activeCameraController_->DrawDebugUI();
	}
}

void CameraManager::SetCbufferCameraResource(UINT RootParameterIndex)
{
	// カメラ用のCBufferをバインド
	DirectXBase::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(RootParameterIndex, cameraResource_->GetGPUVirtualAddress());
}

void CameraManager::SetCbufferProjectionInverseResource(UINT RootParameterIndex)
{
	// カメラの逆行列用のCBufferをバインド
	DirectXBase::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(RootParameterIndex, projectionInverseResource_->GetGPUVirtualAddress());
}

void CameraManager::AddCameraController(const std::string& name, ICameraController* controller)
{
	cameraControllers_[name] = controller;
	
	if (!activeCameraController_)
	{
		activeCameraController_ = controller;
	}
}

ICameraController* CameraManager::GetCameraController(const std::string& name) const
{
	auto it = cameraControllers_.find(name);
	if (it != cameraControllers_.end())
	{
		return it->second;
	}
	return nullptr;
}

void CameraManager::SetActiveCameraController(const std::string& name)
{
	auto it = cameraControllers_.find(name);
	if (it != cameraControllers_.end())
	{
		activeCameraController_ = it->second;
	}
}

void CameraManager::LimitPlayerInFrustum(Vector3& playerLocalPos)
{
	float distance = playerLocalPos.z;

	// ニアクリップより手前、またはファークリップより奥にいる場合は処理しない（任意）
	if (distance < mainCamera_->GetNearClip() || distance > mainCamera_->GetFarClip()) {
		return;
	}

	// 2. カメラのゲッターから視野角(fovY)とアスペクト比を取得
	float fovY = mainCamera_->GetFovY();
	float aspectRatio = mainCamera_->GetAspectRatio();

	// 3. 現在の深さ（distance）における、視錐台の縦・横の限界サイズ（半分の値）を計算
	// ※ fovY がラジアン単位であることを前提としています。度数法の場合は std::tan(fovY * 0.5f * (pi / 180.0f)) にしてください。
	float frustumHeightHalf = distance * std::tan(fovY * 0.5f);
	float frustumWidthHalf = frustumHeightHalf * aspectRatio;

	// 4. 画面ぴったりだとプレイヤーの中心点しか残らないため、
	//    プレイヤーのサイズ分（マージン）を考慮して内側に制限を設定します
	float marginFactor = 0.85f; // 0.0〜1.0 で調整（小さいほど画面の内側に制限される）
	float xLimit = frustumWidthHalf * marginFactor;
	float yLimit = frustumHeightHalf * marginFactor;

	// 5. 計算した限界値でローカルのXとYの座標をクランプ（制限）する
	playerLocalPos.x = std::clamp(playerLocalPos.x, -xLimit, xLimit);
	playerLocalPos.y = std::clamp(playerLocalPos.y, -yLimit, yLimit);
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

void CameraManager::CreateProjectionInverseResource()
{
	// リソース作成
	projectionInverseResource_ = DirectXBase::GetInstance()->CreateConstantBufferResource(sizeof(ProjectionInverse));
	// データを書き込むためのアドレスを取得してDataに割り当てる
	projectionInverseResource_->Map(0, nullptr, reinterpret_cast<void**>(&projectionInverseData_));

	// 初期値を書き込む
	projectionInverseData_->projectionInverse = MakeIdentity4x4();
}

