#pragma once
#include "myMath.h"
#include "DirectXBase.h"

class Camera;

class LightManager
{
public:
	// シングルトンインスタンスの取得
	static LightManager* GetInstance();
	// 終了
	void Finalize();

	// コンストラクタに渡すための鍵
	class ConstructorKey
	{
	private:
		ConstructorKey() = default;
		friend class LightManager;
	};

	// PassKeyを受け取るコンストラクタ
	explicit LightManager(ConstructorKey){}

private: 	// シングルトンインスタンス

	static std::unique_ptr<LightManager> instance_;

	~LightManager() = default;
	LightManager(LightManager&) = delete;
	LightManager& operator=(LightManager&) = delete;

	friend struct std::default_delete<LightManager>;

public:

	static const uint32_t kMaxPointLights = 16; // 最大ポイントライト数

	struct DirectionalLight
	{
		Vector4 color;		//!< ライトの色
		Vector3 direction;	//!< ライトの方向
		float intensity;	//!< 輝度
	};

	struct PointLight
	{
		Vector4 color;		//!< ライトの色
		Vector3 position;	//!< ライトの位置
		float intensity;	//!< 輝度
		float radius;		//!< ライトの届く最大距離
		float decay;		//!< 減衰率	
		float padding[2];
	};

	struct ConstBufferLights 
	{
		DirectionalLight directionalLight;
		PointLight pointLights[kMaxPointLights];
		int32_t numPointLights; // 現在有効なポイントライト数
	};

public:

	void Initialize();

	void Update();

	void SetCBufferLightsResource(UINT RootParameterIndex);

public: // 外部入出力
	
	// 光源リソースのGPUアドレス
	D3D12_GPU_VIRTUAL_ADDRESS GetLightsResourceGPUAddress() const { return LightsResource_->GetGPUVirtualAddress(); }
	// 平行光源の色を取得
	const Vector4& GetDirectionalLightColor() const { return lightsData->directionalLight.color; }
	// 平行光源の方向を取得
	const Vector3& GetDirectionalLightDirection() const { return lightsData->directionalLight.direction; }
	// 平行光源の輝度を取得
	const float& GetDirectionalLightIntensity() const { return lightsData->directionalLight.intensity; }
	// ポイントライトの色を取得
	const Vector4& GetPointLightColor(const uint32_t& index) const { return lightsData->pointLights[index].color; }
	// ポイントライトの位置を取得
	const Vector3& GetPointLightPosition(const uint32_t& index) const { return lightsData->pointLights[index].position; }
	// ポイントライトの輝度を取得
	const float& GetPointLightIntensity(const uint32_t& index) const { return lightsData->pointLights[index].intensity; }
	// ポイントライトの半径を取得
	const float& GetPointLightRadius(const uint32_t& index) const { return lightsData->pointLights[index].radius; }
	// ポイントライトの減衰率を取得
	const float& GetPointLightDecay(const uint32_t& index) const { return lightsData->pointLights[index].decay; }

	// 平行光源の色を設定
	void SetDirectionalLightColor(const Vector4& color){ lightsData->directionalLight.color = color; }
	// 平行光源の方向を設定
	void SetDirectionalLightDirection(const Vector3& direction){ lightsData->directionalLight.direction = direction; }
	// 平行光源の輝度を設定、0.0fで消灯
	void SetDirectionalLightIntensity(const float& intensity){ lightsData->directionalLight.intensity = intensity; }
	// 並行光源の輝度を0.0にして消灯する
	void TurnOffDirectionalLight(){ lightsData->directionalLight.intensity = 0.0f; }
	
	// ポイントライトの色を設定
	void SetPointLightColor(const uint32_t& index, const Vector4& color){ lightsData->pointLights[index].color = color; }
	// ポイントライトの位置を設定
	void SetPointLightPosition(const uint32_t& index, const Vector3& position){ lightsData->pointLights[index].position = position; }
	// ポイントライトの輝度を設定、0.0fで消灯。
	void SetPointLightIntensity(const uint32_t& index, const float& intensity){ lightsData->pointLights[index].intensity = intensity; }
	// ポイントライトの輝度を0.0にして消灯する
	void TurnOffPointLight(const uint32_t& index){ lightsData->pointLights[index].intensity = 0.0f; }
	// ポイントライトの半径を設定
	void SetPointLightRadius(const uint32_t& index, const float& radius){ lightsData->pointLights[index].radius = radius; }
	// ポイントライトの減衰率を設定
	void SetPointLightDecay(const uint32_t& index, const float& decay){ lightsData->pointLights[index].decay = decay; }
	// 現在有効なポイントライト数を設定
	void SetNumPointLights(const uint32_t& numPointLights){ lightsData->numPointLights = numPointLights; }


private:

	// 光源用Resourceを作成
	void CreateLightResource();
	
private:

	// カメラ
	Camera* camera_ = nullptr;

	// 全光源用のリソースを作成
	Microsoft::WRL::ComPtr<ID3D12Resource> LightsResource_ = nullptr;
	// バッファリソース内のデータを指すポインタ
	ConstBufferLights* lightsData = nullptr;
	
};
