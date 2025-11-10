#include "Object3d.h"
#include "Object3dBase.h"
#include "Model.h"
#include "ModelManager.h"

void Object3d::Initialize(Object3dBase* object3dBase)
{

	object3dBase_ = object3dBase;

	

	CreateTransformationMatrixResource();

	CreateDirectionalLightResource();

	

	// Transform変数を作る
	transform_ = { {1.0f, 1.0f, 1.0f}, {0.0f, pi, 0.0f}, {0.0f, 0.0f, 0.0f} };
	cameraTransform_ = { {1.0f, 1.0f, 1.0f}, {0.3f, 0.0f, 0.0f}, {0.0f, 4.0f, -10.0f} };
}

void Object3d::Update()
{
	/*transform_.rotate.y += (1.0f / 180.0f) * pi;*/

	Matrix4x4 worldMatrix = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform_.scale, cameraTransform_.rotate, cameraTransform_.translate);
	Matrix4x4 viewMatrix = Inverse(cameraMatrix);
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, static_cast<float>(WindowsAPI::kClientWidth) / static_cast<float>(WindowsAPI::kClientHeight), 0.1f, 100.0f);

	
	transformationMatrixData_->World = worldMatrix;
	transformationMatrixData_->WVP = worldMatrix * viewMatrix * projectionMatrix;

}

void Object3d::Draw()
{
	// wvp用のCBufferの場所を設定
	object3dBase_->GetDxBase()->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResource_->GetGPUVirtualAddress());

	// 平行光源用のCBufferをバインド（rootParameter[3] = b1）
	object3dBase_->GetDxBase()->GetCommandList()->SetGraphicsRootConstantBufferView(3, DirectionalLightResource_->GetGPUVirtualAddress());	
	
	// 3Dモデルが割り当てられていれば描画する
	if (model_)
	{
		model_->Draw();
	}

}

void Object3d::Finalize()
{

}

void Object3d::SetModel(const std::string& filePath)
{
	model_ = ModelManager::GetInstance()->FindModel(filePath);
}



void Object3d::CreateTransformationMatrixResource()
{

	// 座標変換行列リソースを作成する。Matrix4x4 1つ分のサイズを用意する
	transformationMatrixResource_ = object3dBase_->GetDxBase()->CreateBufferResource(sizeof(TransformationMatrix));
	// TransformationMatrixResourceにデータを書き込むためのアドレスを取得してTransformationMatrixDataに割り当てる
	transformationMatrixResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData_));
	
	// 座標変換行列データの初期値を書き込む
	transformationMatrixData_->World = MakeIdentity4x4();
	transformationMatrixData_->WVP = MakeIdentity4x4();

}

void Object3d::CreateDirectionalLightResource()
{
	// 平行光源リソースを作成する
	DirectionalLightResource_ = object3dBase_->GetDxBase()->CreateBufferResource(sizeof(DirectionalLight));
	// DirectionalLightResourceにデータを書き込むためのアドレスを取得してDirectionalLightDataに割り当てる
	DirectionalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));

	// 平行光源データの初期値を書き込む
	directionalLightData->color = Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
	directionalLightData->direction = Vector3{ 0.0f, -1.0f, 0.0f };
	directionalLightData->intensity = 1.0f;


}
