#include "Object3d.h"
#include "Object3dManager.h"
#include "Model.h"
#include "ModelManager.h"
#include "Camera.h"
#include "LightManager.h"
#include "CameraManager.h"

void Object3d::Initialize()
{

	object3dManager_ = Object3dManager::GetInstance();

	CreateTransformationMatrixResource();

	// Transform変数を作る
	transform_ = { {1.0f, 1.0f, 1.0f}, {0.0f, pi, 0.0f}, {0.0f, 0.0f, 0.0f} };

	psoName_ = Object3dManager::GetInstance()->GetDefaultPsoName();
	
}

void Object3d::Update()
{
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	Matrix4x4 worldViewProjectionMatrix;

	const Matrix4x4& viewProjectionMatrix = CameraManager::GetInstance()->GetActiveCamera()->GetViewProjectionMatrix();
	worldViewProjectionMatrix = Multiply(worldMatrix, viewProjectionMatrix);
	CameraManager::GetInstance()->SetCameraWorldPosition(CameraManager::GetInstance()->GetActiveCamera()->GetTranslate());
	
	transformationMatrixData_->World = worldMatrix;
	transformationMatrixData_->WVP = worldViewProjectionMatrix;
}

void Object3d::Draw()
{
	auto psoSet = PSOManager::GetInstance()->GetPSOData(psoName_, blendMode_, fillMode_);

	// パイプラインステートとルートシグネチャをセット
	DirectXBase::GetInstance()->GetCommandList()->SetPipelineState(psoSet.pipelineState.Get());
	DirectXBase::GetInstance()->GetCommandList()->SetGraphicsRootSignature(psoSet.rootSignature.Get());

	// 形状を設定。PS0に設定しているものとはまた別。同じものを設定すると考えておけば良い
	DirectXBase::GetInstance()->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	// wvp用のCBufferの場所を設定
	DirectXBase::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResource_->GetGPUVirtualAddress());

	// 平行光源用のCBufferをバインド（rootParameter[3] = b1）
	LightManager::GetInstance()->SetCBufferLightsResource(3);

	// カメラ用のCBufferをバインド（rootParameter[4] = b2）
	CameraManager::GetInstance()->SetCbufferCameraResource(4);
	
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

}
