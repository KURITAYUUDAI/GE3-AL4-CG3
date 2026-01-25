#pragma once
#include "DirectXBase.h"

class Camera;

class Object3dManager
{
public:
	// シングルトンインスタンスの取得
	static Object3dManager* GetInstance();
	// 終了
	void Finalize();

public: // メンバ関数
	
	// 初期化
	void Initialize(DirectXBase* dxBase);

	void Update();

	void Draw();

	void DrawingCommon();

public:	// 外部入出力

	// セッター
	void SetDefaultCamera(Camera* camera){ defaultCamera_ = camera; }

	// ゲッター
	DirectXBase* GetDxBase() const { return dxBase_; }
	ID3D12RootSignature* GetRootSignature() { return rootSignature_.Get(); }
	ID3D12PipelineState* GetGraphicsPipeLineState(){ return graphicsPipeLineState_.Get(); }
	Camera* GetDefaultCamera() const { return defaultCamera_; }

private:

	void CreateRootSignature(ID3DBlob* signatureBlob);

	void CreateGraphicsPipelineState();

private: 	// シングルトンインスタンス
	// unique_ptr が delete するために使用する構造体
	struct Deleter
	{
		void operator()(Object3dManager* p) const
		{
			// クラス内部のスコープなので private なデストラクタを呼べる
			delete p;
		}
	};

	// unique_ptr の型定義に Deleter を入れることでdeleteが可能になる
	static std::unique_ptr<Object3dManager, Deleter> instance_;

	Object3dManager() = default;
	~Object3dManager() = default;
	Object3dManager(Object3dManager&) = delete;
	Object3dManager& operator=(Object3dManager&) = delete;

private:

	DirectXBase* dxBase_ = nullptr;

	Camera* defaultCamera_ = nullptr;

	Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignature_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipeLineState_;

};

