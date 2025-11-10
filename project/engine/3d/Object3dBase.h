#pragma once
#include "DirectXBase.h"

class Object3dBase
{

public: // メンバ関数
	
	// 初期化
	void Initialize(DirectXBase* dxBase);

	void Update();

	void Draw();

	void Finalize();

	void DrawingCommon();

public:	// 外部入出力

	DirectXBase* GetDxBase() const { return dxBase_; }
	ID3D12RootSignature* GetRootSignature() { return rootSignature_.Get(); }
	ID3D12PipelineState* GetGraphicsPipeLineState(){ return graphicsPipeLineState_.Get(); }

private:

	void CreateRootSignature(ID3DBlob* signatureBlob);

	void CreateGraphicsPipelineState();

private:

	DirectXBase* dxBase_ = nullptr;

	Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignature_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipeLineState_;
};

