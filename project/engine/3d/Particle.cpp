#include "Particle.h"
#include "ParticleBase.h"
#include "Model.h"
#include "ModelManager.h"
#include "Camera.h"
#include "SrvManager.h"

void Particle::Initialize(ParticleBase* particleBase, const uint32_t& instanceNum)
{
	particleBase_ = particleBase;

	instanceNum_ = instanceNum;

	camera_ = particleBase_->GetDefaultCamera();

	/*CreateTransformationMatrixResource();*/

	/*CreateDirectionalLightResource();*/

	CreateInstancingResource();

	// Transform変数を作る
	transforms_.resize(instanceNum_);
	for (size_t index = 0; index < instanceNum_; ++index)
	{
		transforms_[index] =
		{
			{1.0f, 1.0f, 1.0f},
			{0.0f, pi, 0.0f},
			{ static_cast<float>(index * 0.1f), 
			  static_cast<float>(index * 0.1f), 
			  static_cast<float>(index * 0.1f)  }
		};
	}

}

void Particle::Update()
{
	for (uint32_t index = 0; index < instanceNum_; ++index)
	{
		transforms_[index].rotate.y += (static_cast<float>(index + 1) / 180.0f) * pi;

		Matrix4x4 worldMatrix = MakeAffineMatrix(
			transforms_[index].scale, transforms_[index].rotate, transforms_[index].translate);
		Matrix4x4 worldViewProjectionMatrix;
		if (camera_)
		{
			const Matrix4x4& viewProjectionMatrix = camera_->GetViewProjectionMatrix();
			worldViewProjectionMatrix = Multiply(worldMatrix, viewProjectionMatrix);
		} else
		{
			worldViewProjectionMatrix = worldMatrix;
		}

		instancingData_[index].WVP = worldViewProjectionMatrix;
		instancingData_[index].World = worldMatrix;
	}

}

void Particle::Draw()
{

	//// wvp用のCBufferの場所を設定
	//particleBase_->GetDxBase()->GetCommandList()
	//	->SetGraphicsRootConstantBufferView(1, transformationMatrixResource_->GetGPUVirtualAddress());


	//// 平行光源用のCBufferをバインド（rootParameter[3] = b1）
	//particleBase_->GetDxBase()->GetCommandList()
	//  ->SetGraphicsRootConstantBufferView(3, DirectionalLightResource_->GetGPUVirtualAddress());
	
  particleBase_->GetDxBase()->GetCommandList()
		->SetGraphicsRootDescriptorTable(1, instancingSrvHandleGPU_);

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
	model_->SetInstanceCount(instanceNum_);
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
	// Instancing用のTransformationMatrixResourceを作成する。
	instancingResource_
		= particleBase_->GetDxBase()->CreateBufferResource(sizeof(TransformationMatrix) * instanceNum_);
	// InstancingResourceにデータを書き込むためのアドレスを取得してInstancingDataに割り当てる
	instancingResource_->Map(0, nullptr, reinterpret_cast<void**>(&instancingData_));

	// インスタンスごとの座標変換行列データを書き込む
	for (size_t i = 0; i < instanceNum_; ++i)
	{
		instancingData_[i].World = MakeIdentity4x4();
		instancingData_[i].WVP = MakeIdentity4x4();
	}

	// SRVを作成
	instancingSrvIndex_ = SrvManager::GetInstance()->Allocate();
	instancingSrvHandleCPU_ = SrvManager::GetInstance()->GetCPUDescriptorHandle(instancingSrvIndex_);
	instancingSrvHandleGPU_ = SrvManager::GetInstance()->GetGPUDescriptorHandle(instancingSrvIndex_);

	SrvManager::GetInstance()->CreateSRVforStructuredBuffer(
		instancingSrvIndex_, 
		instancingResource_.Get(),
		instanceNum_, sizeof(TransformationMatrix));
}
