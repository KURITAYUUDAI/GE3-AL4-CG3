#include "LightManager.h"
#include "Camera.h"
#include "ImGuiManager.h"

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
#ifdef _DEBUG
	ImGui::Begin("LightSetting");

	Vector4 directionLightColor = GetDirectionalLightColor();
	Vector3 directionLightDirection = GetDirectionalLightDirection();
	float directionLightIntensity = GetDirectionalLightIntensity();
	if (ImGui::ColorEdit4("LightColor", &directionLightColor.x))
	{
		SetDirectionalLightColor(directionLightColor);
	}
	if (ImGui::DragFloat3("LightDirection", &directionLightDirection.x, 0.01f))
	{
		SetDirectionalLightDirection(Normalize(directionLightDirection));
	}
	if (ImGui::DragFloat("LightIntensity", &directionLightIntensity, 0.01f))
	{
		SetDirectionalLightIntensity(directionLightIntensity);
	}

	Vector4 pointLightColor = GetPointLightColor(0);
	Vector3 pointLightPosition = GetPointLightPosition(0);
	float pointLightIntensity = GetPointLightIntensity(0);
	float pointLightRadius = GetPointLightRadius(0);
	float pointLightDecay = GetPointLightDecay(0);
	if (ImGui::ColorEdit4("PointLightColor", &pointLightColor.x))
	{
		SetPointLightColor(0, pointLightColor);
	}
	if (ImGui::DragFloat3("PointLightPosition", &pointLightPosition.x, 0.01f))
	{
		SetPointLightPosition(0, pointLightPosition);
	}
	if (ImGui::DragFloat("PointLightIntensity", &pointLightIntensity, 0.01f))
	{
		SetPointLightIntensity(0, pointLightIntensity);
	}
	if (ImGui::DragFloat("PointLightRadius", &pointLightRadius, 0.01f))
	{
		SetPointLightRadius(0, pointLightRadius);
	}
	if (ImGui::DragFloat("PointLightDecay", &pointLightDecay, 0.01f))
	{
		SetPointLightDecay(0, pointLightDecay);
	}

	Vector4 spotLightColor = GetSpotLightColor(0);
	Vector3 spotLightPosition = GetSpotLightPosition(0);
	float spotLightIntensity = GetSpotLightIntensity(0);
	Vector3 spotLightDirection = GetSpotLightDirection(0);
	float spotLightDistance = GetSpotLightDistance(0);
	float spotLightDecay = GetSpotLightDecay(0);

	// 角度(度)で保持するローカル変数を用意
	float spotLightAngleDeg = DirectX::XMConvertToDegrees(std::acosf(GetSpotLightCosAngle(0)));
	float spotLightFalloffDeg = DirectX::XMConvertToDegrees(std::acosf(GetSpotLightCosFalloff(0)));

	if (ImGui::ColorEdit4("SpotLightColor", &spotLightColor.x))
	{
		SetSpotLightColor(0, spotLightColor);
	}
	if (ImGui::DragFloat3("SpotLightPosition", &spotLightPosition.x, 0.01f))
	{
		SetSpotLightPosition(0, spotLightPosition);
	}
	if (ImGui::DragFloat("SpotLightIntensity", &spotLightIntensity, 0.01f))
	{
		SetSpotLightIntensity(0, spotLightIntensity);
	}
	if (ImGui::DragFloat("SpotLightDistance", &spotLightDistance, 0.01f))
	{
		SetSpotLightDistance(0, spotLightDistance);
	}
	if (ImGui::DragFloat("SpotLightDecay", &spotLightDecay, 0.01f))
	{
		SetSpotLightDecay(0, spotLightDecay);
	}
	if (ImGui::DragFloat("SpotLightCosAngle", &spotLightAngleDeg, 0.1f))
	{
		SetSpotLightCosAngle(0, std::cos(DirectX::XMConvertToRadians(spotLightAngleDeg)));
	}
	if (ImGui::DragFloat("SpotLightCosFalloff", &spotLightFalloffDeg, 0.1f))
	{
		SetSpotLightCosFalloff(0, std::cos(DirectX::XMConvertToRadians(spotLightFalloffDeg)));
	}

	if (ImGui::Button ("turn off directional light"))
	{
		TurnOffDirectionalLight();
	}

	if (ImGui::Button("turn off point light"))
	{
		TurnOffPointLight(0);
	}

	if (ImGui::Button("turn off spot light"))
	{
		TurnOffSpotLight(0);
	}


	ImGui::End();
#endif
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
		lightsData->pointLights[i].radius = 1.0f;
		lightsData->pointLights[i].decay = 1.0f;
	}

	// 現在有効なポイントライト数の初期値を書き込む
	lightsData->numPointLights = 0;

	// スポットライトの初期値を書き込む
	for (uint32_t i = 0; i < kMaxSpotLights; ++i)
	{
		lightsData->spotLights[i].color = Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
		lightsData->spotLights[i].position = Vector3{ 0.0f, 0.0f, 0.0f };
		lightsData->spotLights[i].intensity = 0.0f;
		lightsData->spotLights[i].direction = Vector3{ 0.0f, -1.0f, 0.0f };
		lightsData->spotLights[i].distance = 1.0f;
		lightsData->spotLights[i].decay = 1.0f;
		lightsData->spotLights[i].cosAngle = std::cos(DirectX::XMConvertToRadians(30.0f));
		// falloffが始まる角度をangleと一致させたら0除算が発生するので注意
		lightsData->spotLights[i].cosFalloff = std::cos(DirectX::XMConvertToRadians(20.0f));
	}

	// 現在有効なスポットライト数の初期値を書き込む
	lightsData->numSpotLights = 0;
}
