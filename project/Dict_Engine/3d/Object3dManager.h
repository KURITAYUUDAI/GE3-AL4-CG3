#pragma once
#include "DirectXBase.h"
#include "PSOManager.h"

class Camera;

class Object3dManager
{
public:
	// シングルトンインスタンスの取得
	static Object3dManager* GetInstance();
	// 終了
	void Finalize();

	// コンストラクタに渡すための鍵
	class ConstructorKey
	{
	private:
		ConstructorKey() = default;
		friend class Object3dManager;
	};

	// PassKeyを受け取るコンストラクタ
	explicit Object3dManager(ConstructorKey){}

private: 	// シングルトンインスタンス

	static std::unique_ptr<Object3dManager> instance_;

	~Object3dManager() = default;
	Object3dManager(Object3dManager&) = delete;
	Object3dManager& operator=(Object3dManager&) = delete;

	friend struct std::default_delete<Object3dManager>;

public: // メンバ関数
	
	// 初期化
	void Initialize(DirectXBase* dxBase);

	void Update();

	void Draw();

	void DrawingCommon();

public:	// 外部入出力

	// セッター
	//void SetDefaultCamera(Camera* camera){ defaultCamera_ = camera; }

	// ゲッター
	const std::string& GetDefaultPsoName() const { return psoName_; }

	DirectXBase* GetDxBase() const { return dxBase_; }
	ID3D12RootSignature* GetRootSignature() { return rootSignature_.Get(); }
	ID3D12PipelineState* GetGraphicsPipeLineState(){ return graphicsPipeLineState_.Get(); }
	//Camera* GetDefaultCamera() const { return defaultCamera_; }

private:

	void CreateRootSignature(ID3DBlob* signatureBlob);

	void CreateGraphicsPipelineState();

private:

	std::string psoName_ = "Object3dDefault";
	PSOManager::BlendMode blendMode_ = PSOManager::BlendMode::Normal;
	PSOManager::FillMode fillMode_ = PSOManager::FillMode::kSolid;

	DirectXBase* dxBase_ = nullptr;

	//Camera* defaultCamera_ = nullptr;

	Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignature_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipeLineState_;

};

