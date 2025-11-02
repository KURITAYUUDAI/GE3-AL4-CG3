#pragma once
#include "DirectXBase.h"

class SpriteBase
{
public:

	void Initialize(DirectXBase* dxBase, 
	ID3DBlob* signatureBlob, D3D12_INPUT_LAYOUT_DESC inputLayoutDesc,
	IDxcBlob* vertexShaderBlob, IDxcBlob* pixelShaderBlob,
	D3D12_BLEND_DESC blendDesc, D3D12_RASTERIZER_DESC rasterizerDesc);

	void Update();

	void Draw();

	void Finalize();

	void DrawingCommon();
	
public: // 外部入力＆出力

	DirectXBase* GetDxBase() const { return dxBase_; }
	ID3D12RootSignature* GetRootSignature() { return rootSignature_.Get(); }
	ID3D12PipelineState* GetGraphicsPipeLineState(){ return graphicsPipeLineState_.Get(); }
	

private:

	void CreateRootSignature(ID3DBlob* signatureBlob);

	void CreateGraphicsPipelineState(
	ID3DBlob* signatureBlob, D3D12_INPUT_LAYOUT_DESC inputLayoutDesc,
	IDxcBlob* vertexShaderBlob, IDxcBlob* pixelShaderBlob,
	D3D12_BLEND_DESC blendDesc, D3D12_RASTERIZER_DESC rasterizerDesc);

private:

	DirectXBase* dxBase_ = nullptr;

	Microsoft::WRL::ComPtr <ID3D12RootSignature> rootSignature_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipeLineState_;


};

