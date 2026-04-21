#include "PSOManager.h"
#include "Logger.h"

std::unique_ptr<PSOManager> PSOManager::instance_ = nullptr;

PSOManager* PSOManager::GetInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = std::make_unique<PSOManager>(ConstructorKey());
	}
	return instance_.get();
}

void PSOManager::Finalize()
{
	psoDatas_.clear();
	rootSignatureDatas_.clear();
	psoConfigs_.clear();
	shaderDatas_.clear();

	instance_.reset();

}

void PSOManager::Initialize()
{
	dxBase_ = DirectXBase::GetInstance();

	psoDatas_.clear();
	rootSignatureDatas_.clear();
	psoConfigs_.clear();
	shaderDatas_.clear();
}

D3D12_STATIC_SAMPLER_DESC PSOManager::GetDefaultStaticSamplerDesc()
{
	D3D12_STATIC_SAMPLER_DESC sampler{};
	sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	sampler.MaxLOD = D3D12_FLOAT32_MAX;
	sampler.ShaderRegister = 0;
	sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	return sampler;
}

const PSOManager::PSOData& PSOManager::GetPSOData(const std::string& name, BlendMode blendMode, FillMode fillMode)
{
	DataKey key{ name, blendMode, fillMode };
	if (psoDatas_.contains(key))
	{
		return psoDatas_[key];
	}
	CreatePipeLineState(name, blendMode, fillMode);
	return psoDatas_[key];
}

void PSOManager::CreatePipeLineState(const std::string& name, BlendMode blend, FillMode fill)
{

	const auto& psoConfig = psoConfigs_.at(name);

	// 1. RootSignatureの設定
	if (!rootSignatureDatas_.contains(name))
	{
		assert(psoConfig.rootSignatureGenerator);
		Logger::Log("RootSignatureGenerator is not set for " + name);
		rootSignatureDatas_[name] = psoConfig.rootSignatureGenerator();
	}
	auto rootSignature = rootSignatureDatas_[name];

	// 2. InputLayoutの設定
	std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDescs;
	if (psoConfig.inputLayoutGenerator)
	{
		inputElementDescs = psoConfig.inputLayoutGenerator();
	}

	// 3. BlendStateの設定
	D3D12_BLEND_DESC blendDesc = CreateBlendDesc(blend);

	// 4. Shaderをコンパイル
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob;
	CompileShader(name, vertexShaderBlob, pixelShaderBlob);

	// 5. RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;		// 裏面（時計回り）を表示しない
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;	// 三角形の中を塗りつぶす
	//rasterizerDesc.DepthClipEnable = true;			// 深度クリッピングを有効にする

	// 6. DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	depthStencilDesc.DepthEnable = psoConfig.depthEnable;
	depthStencilDesc.DepthEnable = true;							// Depthの機能を有効化する
	depthStencilDesc.DepthWriteMask = psoConfig.depthWriteMask;	// 書き込まない
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;	// 比較関数はLessEqual。近ければ描画される

	// 7. PSOを構築
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	{
		// RootSignatureを代入
		graphicsPipelineStateDesc.pRootSignature = rootSignature.Get();	// RootSignature

		// InputLayoutを代入
		graphicsPipelineStateDesc.InputLayout = { inputElementDescs.data(), static_cast<UINT>(inputElementDescs.size()) };

		// BlendStateを代入
		graphicsPipelineStateDesc.BlendState = blendDesc;	// BlendState

		// Shaderを代入
		graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize() };
		graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize() };

		// RasterizerStateを代入
		graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;

		// DepthStencilを代入
		graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
		graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

		// その他の設定
		graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		graphicsPipelineStateDesc.NumRenderTargets = 1;
		graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		graphicsPipelineStateDesc.SampleDesc.Count = 1;
		graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	}
	
	// 8. PSOを生成
	PSOData psoData;
	psoData.rootSignature = rootSignature;
	HRESULT hr = DirectXBase::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&psoData.pipelineState));
	assert(SUCCEEDED(hr) && "Failed to create graphics pipeline state");
	
	// 9. キャッシュに保存
	psoDatas_[{name, blend, fill}] = std::move(psoData);

}

D3D12_BLEND_DESC PSOManager::CreateBlendDesc(BlendMode blendMode)
{
	D3D12_BLEND_DESC blendDesc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;

	// 共通初期値
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;

	switch (blendMode) {
	case BlendMode::None:
		blendDesc.RenderTarget[0].BlendEnable = FALSE;
		break;
	case BlendMode::Normal:
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		break;
	case BlendMode::Add:
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		break;
	case BlendMode::Subtract:
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
		break;
	case BlendMode::Multiply:
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		break;
	case BlendMode::Screen:
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		break;
	}

	return blendDesc;
}

void PSOManager::CompileShader(const std::string& name, Microsoft::WRL::ComPtr<IDxcBlob>& outVS, Microsoft::WRL::ComPtr<IDxcBlob>& outPS)
{
	// キャッシュを確認してあれば渡す
	if (shaderDatas_.contains(name))
	{
		outVS = shaderDatas_[name].vertexShader;
		outPS = shaderDatas_[name].pixelShader;
		return;
	}

	// 無い場合は新規でコンパイル
	psoConfigs_[name];
	
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob;

	vertexShaderBlob = DirectXBase::GetInstance()->CompileShader(psoConfigs_[name].vertexShaderPath, L"vs_6_0");
	pixelShaderBlob = DirectXBase::GetInstance()->CompileShader(psoConfigs_[name].pixelShaderPath, L"ps_6_0");
	
	assert(vertexShaderBlob && pixelShaderBlob);

	shaderDatas_[name] = { vertexShaderBlob, pixelShaderBlob };

	outVS = vertexShaderBlob;
	outPS = pixelShaderBlob;
}
