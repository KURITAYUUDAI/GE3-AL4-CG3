#include "DebugCamera.h"
#include "InputManager.h"
#include "ImGuiManager.h"

void DebugCamera::Initialize()
{
	Camera::Initialize();
}

void DebugCamera::Update()
{
#ifdef _DEBUG

	ImGui::Begin("DebugCameraWindow");

	if (InputManager::GetInstance()->PushMouse(2) && !ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow))
	{
		// ウィンドウ上ではない領域でのドラッグ
		ImVec2 delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Middle, 0.0f);

		// カメラ角度に反映
		phi += delta.x * rotationSpeed;
		theta += delta.y * rotationSpeed;

		if (phi > pi)
		{
			phi = phi - 2.0f * pi;
		} else if (phi < -pi)
		{
			phi = phi + 2.0f * pi;
		}

		// ピッチを上下90°未満にクランプ
		const float limit = 0.4999f * pi; // 約85°
		theta = std::clamp(theta, -limit, limit);

		ImGui::ResetMouseDragDelta(ImGuiMouseButton_Middle);

		ImGui::Text("Delta x : %3.6f, y : %3.6f", delta.x, delta.y);

	}

	LONG wheel = InputManager::GetInstance()->MouseWheel(); // 前フレームとの差分
	radius -= wheel * 0.005f;              // 感度は 0.1f などで調整
	radius = max(1.0f, min(radius, 20.0f)); // クランプして近づきすぎ防止

	transform_.rotate.x = phi;
	transform_.rotate.y = theta;

	transform_.translate.x = radius * std::cos(theta) * std::sin(phi);
	transform_.translate.y = radius * std::sin(theta);
	transform_.translate.z = radius * std::cos(theta) * std::cos(phi);

	viewMatrix_ = MakeLookAtMatrix(transform_.translate, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });

	ImGui::End();

	worldMatrix_ = Inverse(viewMatrix_);

	projectionMatrix_ = MakePerspectiveFovMatrix(fovY_, aspectRatio_, nearClip_, farClip_);

	Matrix4x4 backToFrontMatrix = MakeRotateYMatrix(pi);
	billboardMatrix_ = Multiply(backToFrontMatrix, worldMatrix_);
	billboardMatrix_.m[3][0] = 0.0f;
	billboardMatrix_.m[3][1] = 0.0f;
	billboardMatrix_.m[3][2] = 0.0f;

	viewProjectionMatrix_ = Multiply(viewMatrix_, projectionMatrix_);

#endif
}