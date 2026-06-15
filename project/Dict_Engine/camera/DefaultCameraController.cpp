#include "DefaultCameraController.h"
#include "Camera.h"
#include "ImGuiManager.h"

void DefaultCameraController::Initialize()
{
	worldTransform_.Initialize();

	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
	worldTransform_.SetRotate({0.0f, 0.0f, 0.0f});
	worldTransform_.translate_ = {0.0f, 0.0f, -15.0f};
}

void DefaultCameraController::Update(Camera* mainCamera, const float& deltaTime)
{
	if (!mainCamera) return;

#ifdef _DEBUG

	ImGui::Begin("CameraSetting");
	Vector3 cameraRotate = worldTransform_.GetRotate();
	Vector3 cameraTranslate = worldTransform_.translate_;
	if (ImGui::DragFloat3("CameraRotate", &cameraRotate.x, (1.0f / 180.0f) * pi))
	{
		worldTransform_.SetRotate(cameraRotate);
	}
	if (ImGui::DragFloat3("CameraTranslate", &cameraTranslate.x, 0.1f))
	{
		worldTransform_.translate_ = cameraTranslate;
	}

	ImGui::End();

#endif

	worldTransform_.UpdateMatrix();

	mainCamera->SetRotate(worldTransform_.GetRotate());
	mainCamera->SetTranslate(worldTransform_.translate_);
}

void DefaultCameraController::Finalize()
{

}

void DefaultCameraController::DrawDebugUI()
{

}
