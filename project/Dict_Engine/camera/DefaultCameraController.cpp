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
