#pragma once
#include "DirectXBase.h"
#include "myMath.h"

class Model;

class Camera;

class SrvManager;

class ParticleManager
{
public:

	struct MaterialData
	{
		std::string textureFilePath;
		uint32_t textureIndex = 0;
		Vector4 color;
	};

	struct TransformationMatrix
	{
		Matrix4x4 WVP;
		Matrix4x4 World;
	};

	struct ParticleForGPU
	{
		Matrix4x4 WVP;
		Matrix4x4 World;
		Vector4 color;
	};

	struct DirectionalLight
	{
		Vector4 color;		//!< ライトの色
		Vector3 direction;	//!< ライトの方向
		float intensity;	//!< 輝度
	};

	struct Particle
	{
		Transform transform;	// 変換情報
		Vector3 velocity;		// 速度
		Vector4 color;			// 色
		float lifeTime;			// 生存時間
		float currentTime;		// 経過時間
	};

	struct Emitter
	{
		Transform transform;
		uint32_t count;
		float frequency;
		float frequencyTime;
	};

	struct AccelerationField
	{
		Vector3 acceleration;
		AABB area;
	};
	
	struct ParticleGroup
	{
		MaterialData materialData;									// マテリアルデータ
		std::list<Particle> particles;								// パーティクルリスト
		uint32_t instanceNum;
		uint32_t maxInstanceNum;									// インスタンス数
		Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource;	// インスタンスバッファリソース
		ParticleForGPU* instancingData = nullptr;					// インスタンスデータを書き込むためのポインタ
		uint32_t instancingSrvIndex;								// インスタンス用SRVのインデックス
	};

public:

	// シングルトンインスタンスの取得
	static ParticleManager* GetInstance();
	// 終了
	void Finalize();

public:

	// 初期化
	void Initialize(DirectXBase* dxBase);

	void Update();

	void Draw();

	

	void DrawingCommon();

	void CreateParticleGroup(const std::string name, const std::string textureFilePath);

	void Emit(const std::string name, const Vector3& position, uint32_t count);

	void CreateAccelerationField(const Vector3& acceleration, const AABB& area);

public: // 外部入出力

	// セッター
	void SetDefaultCamera(Camera* camera){ defaultCamera_ = camera; }
	void SetModel(const std::string& filePath);

	// ゲッター
	DirectXBase* GetDxBase() const { return dxBase_; }
	ID3D12RootSignature* GetRootSignature() { return rootSignature_.Get(); }
	ID3D12PipelineState* GetGraphicsPipeLineState(){ return graphicsPipeLineState_.Get(); }
	Camera* GetDefaultCamera() const { return defaultCamera_; }

	const std::list<std::string> GetParticleGroupName();

private:

	static ParticleManager* instance_;

	ParticleManager() = default;
	~ParticleManager() = default;
	ParticleManager(ParticleManager&) = delete;
	ParticleManager& operator=(ParticleManager&) = delete;

private:

	void CreateRootSignature(ID3DBlob* signatureBlob);

	void CreateGraphicsPipelineState();

	Particle MakeNewParticle(const Vector3& translate);

private:

	DirectXBase* dxBase_ = nullptr;

	SrvManager* srvManager_ = nullptr;

	Camera* defaultCamera_ = nullptr;

	Model* model_ = nullptr;

	Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignature_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipeLineState_;

	// テクスチャグループのコンテナ
	std::unordered_map<std::string, ParticleGroup> particleGroups_;

	// 場のコンテナ
	std::list<AccelerationField> accelerationFields_;

	// カメラのビュープロジェクション
	Matrix4x4 viewProjectionMatrix_;

};

