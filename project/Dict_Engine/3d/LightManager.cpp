#include "LightManager.h"
#include "Camera.h"

std::unique_ptr<LightManager> LightManager::instance_ = nullptr;

LightManager* LightManager::GetInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = std::make_unique<LightManager>(ConstructorKey());
	}
	return instance_.get();
}

void LightManager::Finalize()
{
	instance_.reset();
}

void LightManager::Initialize()
{
	CreateLightResource();
}

void LightManager::Update()
{
	
}

void LightManager::SetCBufferLightsResource(UINT RootParameterIndex)
{
	DirectXBase::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(RootParameterIndex, LightsResource_->GetGPUVirtualAddress());
}

void LightManager::CreateLightResource()
{
	// 光源リソースを作成する
	LightsResource_ = DirectXBase::GetInstance()->CreateBufferResource(sizeof(ConstBufferLights));
	// LightResourceにデータを書き込むためのアドレスを取得してlightDataに割り当てる
	LightsResource_->Map(0, nullptr, reinterpret_cast<void**>(&lightsData));

	// 平行光源データの初期値を書き込む
	lightsData->directionalLight.color = Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
	lightsData->directionalLight.direction = Vector3{ 0.0f, -1.0f, 0.0f };
	lightsData->directionalLight.intensity = 0.0f;

	// ポイントライトの初期値を書き込む
	for (uint32_t i = 0; i < kMaxPointLights; ++i)
	{
		lightsData->pointLights[i].color = Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
		lightsData->pointLights[i].position = Vector3{ 0.0f, 0.0f, 0.0f };
		lightsData->pointLights[i].intensity = 0.0f;
	}

	// 現在有効なポイントライト数の初期値を書き込む
	lightsData->numPointLights = 0;
}
