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

public:
	// コンストラクタに渡すための鍵
	class ConstructorKey
	{
	private:
		ConstructorKey() = default;
		friend class SpriteManager;
	};

	// PassKeyを受け取るコンストラクタ
	explicit SpriteManager(ConstructorKey){}

private: 	// シングルトンインスタンス

	static std::unique_ptr<SpriteManager> instance_;

	~SpriteManager() = default;
	SpriteManager(SpriteManager&) = delete;
	SpriteManager& operator=(SpriteManager&) = delete;

	friend struct std::default_delete<SpriteManager>;

private:

	DirectXBase* dxBase_ = nullptr;

	Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignature_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipeLineState_;


};

