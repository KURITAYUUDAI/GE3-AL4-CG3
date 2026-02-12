#include "Object3d.h"
#include "Object3dManager.h"
#include "Model.h"
#include "ModelManager.h"
#include "Camera.h"

void Object3d::Initialize()
{

	object3dManager_ = Object3dManager::GetInstance();

	camera_ = object3dManager_->GetDefaultCamera();

	CreateTransformationMatrixResource();

	CreateDirectionalLightResource();

	CreateCameraResource();

	// Transform変数を作る
	transform_ = { {1.0f, 1.0f, 1.0f}, {0.0f, pi, 0.0f}, {0.0f, 0.0f, 0.0f} };
	
}

void Object3d::Update()
{
	/*transform_.rotate.y += (5.0f / 180.0f) * pi;*/

	Matrix4x4 worldMatrix = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	Matrix4x4 worldViewProjectionMatrix;
	if (camera_)
	{
		const Matrix4x4& viewProjectionMatrix = camera_->GetViewProjectionMatrix();
		worldViewProjectionMatrix = Multiply(worldMatrix, viewProjectionMatrix);
		cameraData_->worldPosition = camera_->GetTranslate();
	}
	else
	{
		worldViewProjectionMatrix = worldMatrix;
	}
	
	transformationMatrixData_->World = worldMatrix;
	transformationMatrixData_->WVP = worldViewProjectionMatrix;
	transformationMatrixData_->WorldInverseTranspose = Inverse(Transpose(worldMatrix));

	
}

void Object3d::Draw()
{
	// wvp用のCBufferの場所を設定
	object3dManager_->GetDxBase()->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResource_->GetGPUVirtualAddress());

	// 平行光源用のCBufferをバインド（rootParameter[3] = b1）
	object3dManager_->GetDxBase()->GetCommandList()->SetGraphicsRootConstantBufferView(3, DirectionalLightResource_->GetGPUVirtualAddress());	
	
	// カメラ用のCBufferをバインド（rootParameter[4] = b2）
	object3dManager_->GetDxBase()->GetCommandList()->SetGraphicsRootConstantBufferView(4, cameraResource_->GetGPUVirtualAddress());

	// 3Dモデルが割り当てられていれば描画する
	if (model_)
	{
		model_->Draw(1);
	}

}

void Object3d::Finalize()
{

}

void Object3d::SetModel(const std::string& filePath)
{
	model_ = ModelManager::GetInstance()->FindModel(filePath);
	enableLighting_ = model_->GetEnableLighting();
}

void Object3d::SetEnableLighting(const int32_t& enableLighting)
{
	enableLighting_ = enableLighting;
	if (model_)
	{
		model_->SetEnableLighting(enableLighting_);
	}
}

void Object3d::CreateTransformationMatrixResource()
{

	// 座標変換行列リソースを作成する。Matrix4x4 1つ分のサイズを用意する
	transformationMatrixResource_ = object3dManager_->GetDxBase()->CreateBufferResource(sizeof(TransformationMatrix));
	// TransformationMatrixResourceにデータを書き込むためのアドレスを取得してTransformationMatrixDataに割り当てる
	transformationMatrixResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData_));
	
	// 座標変換行列データの初期値を書き込む
	transformationMatrixData_->World = MakeIdentity4x4();
	transformationMatrixData_->WVP = MakeIdentity4x4();
	transformationMatrixData_->WorldInverseTranspose = MakeIdentity4x4();

}

void Object3d::CreateDirectionalLightResource()
{
	// 平行光源リソースを作成する
	DirectionalLightResource_ = object3dManager_->GetDxBase()->CreateBufferResource(sizeof(DirectionalLight));
	// DirectionalLightResourceにデータを書き込むためのアドレスを取得してDirectionalLightDataに割り当てる
	DirectionalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));

	// 平行光源データの初期値を書き込む
	directionalLightData->color = Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
	directionalLightData->direction = Vector3{ 0.0f, -1.0f, 0.0f };
	directionalLightData->intensity = 1.0f;
}

void Object3d::CreateCameraResource()
{

	// カメラ用リソースを作成する
	cameraResource_ = object3dManager_->GetDxBase()->CreateBufferResource(sizeof(CameraForGPU));
	// CameraResourceにデータを書き込むためのアドレスを取得してCameraDataに割り当てる
	cameraResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraData_));

	// カメラデータの初期値を書き込む
	cameraData_->worldPosition = camera_->GetTranslate();
}
