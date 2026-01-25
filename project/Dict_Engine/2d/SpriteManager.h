#pragma once
#include "DirectXBase.h"

class SpriteManager
{
public:
	// シングルトンインスタンスの取得
	static SpriteManager* GetInstance();
	// 終了
	void Finalize();

public:

	void Initialize(DirectXBase* dxBase);

	void Update();

	void Draw();

	void DrawingCommon();
	
public: // 外部入力＆出力

	DirectXBase* GetDxBase() const { return dxBase_; }
	ID3D12RootSignature* GetRootSignature() { return rootSignature_.Get(); }
	ID3D12PipelineState* GetGraphicsPipeLineState(){ return graphicsPipeLineState_.Get(); }
	

private:

	void CreateRootSignature(ID3DBlob* signatureBlob);

	void CreateGraphicsPipelineState();

private: 	// シングルトンインスタンス
	// unique_ptr が delete するために使用する構造体
	struct Deleter
	{
		void operator()(SpriteManager* p) const
		{
			// クラス内部のスコープなので private なデストラクタを呼べる
			delete p;
		}
	};

	// unique_ptr の型定義に Deleter を入れることでdeleteが可能になる
	static std::unique_ptr<SpriteManager, Deleter> instance_;

	SpriteManager() = default;
	~SpriteManager() = default;
	SpriteManager(SpriteManager&) = delete;
	SpriteManager& operator=(SpriteManager&) = delete;

private:

	DirectXBase* dxBase_ = nullptr;

	Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignature_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipeLineState_;


};

