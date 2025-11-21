#include "Particle.h"
#include "ParticleBase.h"
#include "Model.h"
#include "ModelManager.h"
#include "Camera.h"
#include "TextureManager.h"

void Particle::Initialize(ParticleBase* particleBase)
{
	particleBase_ = particleBase;

	camera_ = particleBase_->GetDefaultCamera();

	CreateTransformationMatrixResource();

	/*CreateDirectionalLightResource();*/

	CreateInstancingResource();

	TextureManager::GetInstance()->LoadInstancingTexture(
		"Resources/Textures/uvChecker.png", kNumInstance_, sizeof(TransformationMatrix));

	// Transform変数を作る
	transform_ = { {1.0f, 1.0f, 1.0f}, {0.0f, pi, 0.0f}, {0.0f, 0.0f, 0.0f} };
}

void Particle::Update()
{
	transform_.rotate.y += (5.0f / 180.0f) * pi;

	Matrix4x4 worldMatrix = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	Matrix4x4 worldViewProjectionMatrix;
	if (camera_)
	{
		const Matrix4x4& viewProjectionMatrix = camera_->GetViewProjectionMatrix();
		worldViewProjectionMatrix = Multiply(worldMatrix, viewProjectionMatrix);
	} else
	{
		worldViewProjectionMatrix = worldMatrix;
	}

	transformationMatrixData_->World = worldMatrix;
	transformationMatrixData_->WVP = worldViewProjectionMatrix;
}

void Particle::Draw()
{
	// wvp用のCBufferの場所を設定
	particleBase_->GetDxBase()->GetCommandList()
		->SetGraphicsRootConstantBufferView(1, transformationMatrixResource_->GetGPUVirtualAddress());

	//// 平行光源用のCBufferをバインド（rootParameter[3] = b1）
	//particleBase_->GetDxBase()->GetCommandList()
	//  ->SetGraphicsRootConstantBufferView(3, DirectionalLightResource_->GetGPUVirtualAddress());


	// 3Dモデルが割り当てられていれば描画する
	if (model_)
	{
		model_->Draw();
	}

}

void Particle::Finalize()
{

}

void Particle::SetModel(const std::string& filePath)
{
	model_ = ModelManager::GetInstance()->FindModel(filePath);
}

void Particle::SetModelInstanceCount(const UINT instanceCount)
{
	model_->SetInstanceCount(instanceCount);
}

const UINT& Particle::GetModelInstanceCount() const
{
	return model_->GetInstanceCount();
}

void Particle::CreateTransformationMatrixResource()
{
	// 座標変換行列リソースを作成する。Matrix4x4 1つ分のサイズを用意する
	transformationMatrixResource_ = particleBase_->GetDxBase()->CreateBufferResource(sizeof(TransformationMatrix));
	// TransformationMatrixResourceにデータを書き込むためのアドレスを取得してTransformationMatrixDataに割り当てる
	transformationMatrixResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData_));

	// 座標変換行列データの初期値を書き込む
	transformationMatrixData_->World = MakeIdentity4x4();
	transformationMatrixData_->WVP = MakeIdentity4x4();
}

//void Particle::CreateDirectionalLightResource()
//{
//	// 平行光源リソースを作成する
//	DirectionalLightResource_ = particleBase_->GetDxBase()->CreateBufferResource(sizeof(DirectionalLight));
//	// DirectionalLightResourceにデータを書き込むためのアドレスを取得してDirectionalLightDataに割り当てる
//	DirectionalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));
//
//	// 平行光源データの初期値を書き込む
//	directionalLightData->color = Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
//	directionalLightData->direction = Vector3{ 0.0f, -1.0f, 0.0f };
//	directionalLightData->intensity = 1.0f;
//}

void Particle::CreateInstancingResource()
{
	kNumInstance_ = 10;

	// Instancing用のTransformationMatrixResourceを作成する。
	instancingResource_
		= particleBase_->GetDxBase()->CreateBufferResource(sizeof(TransformationMatrix) * kNumInstance_);
	// InstancingResourceにデータを書き込むためのアドレスを取得してInstancingDataに割り当てる
	instancingResource_->Map(0, nullptr, reinterpret_cast<void**>(&instancingDara_));

	// インスタンスごとの座標変換行列データを書き込む
	for (size_t i = 0; i < kNumInstance_; ++i)
	{
		instancingDara_[i].World = MakeIdentity4x4();
		instancingDara_[i].WVP = MakeIdentity4x4();
	}
}
