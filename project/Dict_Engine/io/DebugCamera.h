#pragma once
#include "../math/myMath.h"
#include <Windows.h>

class InputManager;

/// <summary>
///  デバッグカメラ
/// </summary>
class DebugCamera
{
public:

	InputManager* input_;

	/// <summary>
	///  初期化
	/// </summary>
	void Initialize();

	/// <summary>
	///  更新
	/// </summary>
	void Update(InputManager* input, Transform camera);

	Matrix4x4 GetViewMatrix() { return viewMatrix_; }

	Matrix4x4 GetWorldMatrix() { return worldMatrix_; }


private:

	// カメラ
	Transform debugCamera_;
	// ワールド行列
	Matrix4x4 worldMatrix_ = MakeIdentity4x4();
	// ビュー行列
	Matrix4x4 viewMatrix_ = MakeIdentity4x4();
	// 射影行列
	Matrix4x4 projectionMatrix_ = MakeIdentity4x4();

	float radius = 6.0f;    // 原点からの距離
	const float rotationSpeed = 0.005f; // ドラッグ速度の調整係数

	float theta = 0.0f;
	float phi = 0.0f;

	// マウス座標を取得
	int mouseX, mouseY;

};
