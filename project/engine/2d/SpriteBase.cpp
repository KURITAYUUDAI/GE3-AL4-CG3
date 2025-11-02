#include "SpriteBase.h"

void SpriteBase::Initialize(DirectXBase* dxBase,
	ID3DBlob* signatureBlob, D3D12_INPUT_LAYOUT_DESC inputLayoutDesc,
	IDxcBlob* vertexShaderBlob, IDxcBlob* pixelShaderBlob,
	D3D12_BLEND_DESC blendDesc, D3D12_RASTERIZER_DESC rasterizerDesc)
{

	dxBase_ = dxBase;

	CreateGraphicsPipelineState(
		signatureBlob, inputLayoutDesc, vertexShaderBlob, 
		pixelShaderBlob, blendDesc, rasterizerDesc);
}

void SpriteBase::Update()
{

}

void SpriteBase::Draw()
{

}

void SpriteBase::Finalize()
{

}

void SpriteBase::DrawingCommon()
{
	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	dxBase_->GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());
	dxBase_->GetCommandList()->SetPipelineState(graphicsPipeLineState_.Get());	// PS0を設定

	// 形状を設定。PS0に設定しているものとはまた別。同じものを設定すると考えておけば良い
	dxBase_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

void SpriteBase::CreateRootSignature(ID3DBlob* signatureBlob)
{
	HRESULT hr;
	hr = dxBase_->GetDevice()->CreateRootSignature(0,
		signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(hr));

}

void SpriteBase::CreateGraphicsPipelineState(
	ID3DBlob* signatureBlob, D3D12_INPUT_LAYOUT_DESC inputLayoutDesc, 
	IDxcBlob* vertexShaderBlob, IDxcBlob* pixelShaderBlob,
	D3D12_BLEND_DESC blendDesc, D3D12_RASTERIZER_DESC rasterizerDesc)
{

	CreateRootSignature(signatureBlob);

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};

	// DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	
	// Depthの機能を有効化する
	depthStencilDesc.DepthEnable = false;
	
	// 書き込みします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

	// DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_UNKNOWN;

	graphicsPipelineStateDesc.pRootSignature = rootSignature_.Get();	// RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;	// InputLayout
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
	vertexShaderBlob->GetBufferSize() };	// VertexShader
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
	pixelShaderBlob->GetBufferSize() };		// PixelShader
	graphicsPipelineStateDesc.BlendState = blendDesc;	// BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;	// RasterizerState
	
	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	
	// 利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	
	// どのように画面に色を打ち込むかの設定（気にしなくて良い）
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	// 実際に生成
	HRESULT hr;
	hr = dxBase_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipeLineState_));
	assert(SUCCEEDED(hr));

}