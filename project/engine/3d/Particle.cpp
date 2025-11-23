#include "Particle.h"
#include "ParticleBase.h"
#include "Model.h"
#include "ModelManager.h"
#include "Camera.h"
#include "SrvManager.h"
#include "SeedManager.h"
#include "InputManager.h"

void Particle::Initialize(ParticleBase* particleBase, const uint32_t& instanceNum)
{
	particleBase_ = particleBase;

	maxInstanceNum_ = instanceNum;

	camera_ = particleBase_->GetDefaultCamera();

	/*CreateTransformationMatrixResource();*/

	/*CreateDirectionalLightResource();*/

	CreateInstancingResource();

	// Transform変数を作る
	particles_.resize(maxInstanceNum_);
	for (size_t index = 0; index < maxInstanceNum_; ++index)
	{
		particles_[index].transform =
		{
			{1.0f, 1.0f, 1.0f},
			{0.0f, 0.0f, 0.0f},
			{ 
				SeedManager::GetInstance()->GenerateFloat(-1.0f, 1.0f),
				SeedManager::GetInstance()->GenerateFloat(-1.0f, 1.0f),
				SeedManager::GetInstance()->GenerateFloat(-1.0f, 1.0f),  
			}
		};

		particles_[index].velocity =
		{ 
			SeedManager::GetInstance()->GenerateFloat(-1.0f, 1.0f),
			SeedManager::GetInstance()->GenerateFloat(-1.0f, 1.0f),
			SeedManager::GetInstance()->GenerateFloat(-1.0f, 1.0f),
		};

		particles_[index].color =
		{
			SeedManager::GetInstance()->GenerateFloat(0.0f, 1.0f),
			SeedManager::GetInstance()->GenerateFloat(0.0f, 1.0f),
			SeedManager::GetInstance()->GenerateFloat(0.0f, 1.0f),
			1.0f
		};

		particles_[index].lifeTime = SeedManager::GetInstance()->GenerateFloat(1.0f, 3.0f);

		particles_[index].currentTime = 0.0f;
	}

}

void Particle::Update()
{
	instanceNum_ = 0;

	for (uint32_t index = 0; index < maxInstanceNum_; ++index)
	{
		if (particles_[index].lifeTime <= particles_[index].currentTime)
		{
			continue;
		}

		/*particles_[index].transform.translate += particles_[index].velocity * kDeltaTime;*/

		/*transforms_[index].rotate.y += (static_cast<float>(index + 1) / 180.0f) * pi;*/

		particles_[index].color.w =
			1.0f - (particles_[index].currentTime / particles_[index].lifeTime);

		/*particles_[index].currentTime += kDeltaTime;*/

		Matrix4x4 worldMatrix;

		if (InputManager::GetInstance()->PushKey(DIK_1))
		{
			worldMatrix = camera_->GetBillboardWorldMatrix(
				particles_[index].transform.scale,
				particles_[index].transform.translate);
		}
		else
		{
			worldMatrix = MakeAffineMatrix(
			particles_[index].transform.scale,
			particles_[index].transform.rotate,
			particles_[index].transform.translate);
		}

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
		instancingData_[index].color = particles_[index].color;

		++instanceNum_;
	}

}

void Particle::Draw()
{
	model_->SetTexture("resources", "circle.png");
	model_->SetInstanceCount(instanceNum_);

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
	model_->SetInstanceCount(maxInstanceNum_);
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
		= particleBase_->GetDxBase()->CreateBufferResource(sizeof(ParticleForGPU) * maxInstanceNum_);
	// InstancingResourceにデータを書き込むためのアドレスを取得してInstancingDataに割り当てる
	instancingResource_->Map(0, nullptr, reinterpret_cast<void**>(&instancingData_));

	// インスタンスごとの座標変換行列データを書き込む
	for (size_t i = 0; i < maxInstanceNum_; ++i)
	{
		instancingData_[i].World = MakeIdentity4x4();
		instancingData_[i].WVP = MakeIdentity4x4();
		instancingData_[i].color = Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
	}

	// SRVを作成
	instancingSrvIndex_ = SrvManager::GetInstance()->Allocate();
	instancingSrvHandleCPU_ = SrvManager::GetInstance()->GetCPUDescriptorHandle(instancingSrvIndex_);
	instancingSrvHandleGPU_ = SrvManager::GetInstance()->GetGPUDescriptorHandle(instancingSrvIndex_);

	SrvManager::GetInstance()->CreateSRVforStructuredBuffer(
		instancingSrvIndex_, 
		instancingResource_.Get(),
		maxInstanceNum_, sizeof(ParticleForGPU));
}
