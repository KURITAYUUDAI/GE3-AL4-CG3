#include "DebugCamera.h"
#include "GetKeyMouse.h"
#include "../../externals/imgui/imgui.h"

void DebugCamera::Initialize()
{

	viewMatrix_ = MakeIdentity4x4();
	projectionMatrix_ = MakeIdentity4x4();
}

void DebugCamera::Update(GetKey getKey, Transform originCamera)
{
	getKey_ = &getKey;

	Vector3 move = { 0.0f, 0.0f, 0.0f };

	Transform camera = { 0.0f, 0.0f, 0.0f };

	if (getKey_->IsPress(DIK_RIGHT))
	{
		debugCamera_.rotate.y += 0.5f / 180.0f * static_cast<float>(M_PI);
	}
	else if (getKey_->IsPress(DIK_LEFT))
	{
		debugCamera_.rotate.y -= 0.5f / 180.0f * static_cast<float>(M_PI);
	}

	if (getKey_->IsPress(DIK_UP))
	{
		debugCamera_.rotate.x += 0.5f / 180.0f * static_cast<float>(M_PI);
	}
	else if (getKey_->IsPress(DIK_DOWN))
	{
		debugCamera_.rotate.x -= 0.5f / 180.0f * static_cast<float>(M_PI);
	}

	camera.rotate.x = debugCamera_.rotate.x + originCamera.rotate.x;
	camera.rotate.y = debugCamera_.rotate.y + originCamera.rotate.y;
	camera.rotate.z = debugCamera_.rotate.z + originCamera.rotate.z;

	if (getKey_->IsPress(DIK_W))
	{
		const float speed = 0.5f;
		move.z = speed;
	}
	else if (getKey_->IsPress(DIK_S))
	{
		const float speed = -0.5f;
		move.z = speed;
	}

	if (getKey_->IsPress(DIK_A))
	{
		const float speed = 0.5f;
		move.x = speed;
	}
	else if (getKey_->IsPress(DIK_D))
	{
		const float speed = -0.5f;
		move.x = speed;
	}

	if (getKey_->IsPress(DIK_R))
	{
		const float speed = 0.5f;
		move.y = speed;
	}
	else if (getKey_->IsPress(DIK_F))
	{
		const float speed = -0.5f;
		move.y = speed;
	}

	Matrix4x4 rotMat = MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, camera.rotate, { 0.0f, 0.0f, 0.0f });

	Vector3 rotatedMove =
	{
		move.x * rotMat.m[0][0] + move.y * rotMat.m[1][0] + move.z * rotMat.m[2][0],
		move.x * rotMat.m[0][1] + move.y * rotMat.m[1][1] + move.z * rotMat.m[2][1],
		move.x * rotMat.m[0][2] + move.y * rotMat.m[1][2] + move.z * rotMat.m[2][2]
	};

	debugCamera_.translate.x += rotatedMove.x;
	debugCamera_.translate.y += rotatedMove.y;
	debugCamera_.translate.z += rotatedMove.z;
	
	camera.translate.x = debugCamera_.translate.x + originCamera.translate.x;
	camera.translate.y = debugCamera_.translate.y + originCamera.translate.y;
	camera.translate.z = debugCamera_.translate.z + originCamera.translate.z;

	viewMatrix_ = Inverse(MakeAffineMatrix(originCamera.scale, camera.rotate, camera.translate));
}
