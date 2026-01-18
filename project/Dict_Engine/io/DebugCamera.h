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

	Matrix4x4 GetViewMatrix() { return viewMatrix_;}


private:

	// カメラ
	Transform debugCamera_;

	// ビュー行列
	Matrix4x4 viewMatrix_ = MakeIdentity4x4();
	// 射影行列
	Matrix4x4 projectionMatrix_ = MakeIdentity4x4();

};

