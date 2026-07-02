#include "TextManager.h"
#include "Logger.h"

std::unique_ptr<TextManager> TextManager::instance_ = nullptr;

TextManager* TextManager::GetInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = std::make_unique<TextManager>(ConstructorKey());
	}

	return instance_.get();
}

void TextManager::Finalize()
{
	instance_.reset();
}

void TextManager::Initialize()
{
    RegisterPSO();
}

void TextManager::RegisterPSO()
{
    // PSOの設定
    PSOManager::PSOConfig config{};
    config.vertexShaderPath = L"resources/shaders/Font/FreetypeFont.VS.hlsl";
    config.pixelShaderPath = L"resources/shaders/Font/FreetypeFont.PS.hlsl";

    // RootSignatureの設定
    config.rootSignatureGenerator = [](){
        std::vector<D3D12_ROOT_PARAMETER> rootParameters;
        std::vector<D3D12_STATIC_SAMPLER_DESC> staticSamplerDescs;
        D3D12_STATIC_SAMPLER_DESC sampler{};
        sampler = PSOManager::GetInstance()->GetDefaultStaticSamplerDesc();
        staticSamplerDescs.push_back(sampler);

        D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
        descriptorRange[0].BaseShaderRegister = 0; // 0から始まる
        descriptorRange[0].NumDescriptors = 1;	// 数は一つ
        descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRVを使う
        descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offsetを自動計算

        rootParameters.resize(3);

        // Enum定義 (可読性のため)
        enum {
            kMaterial, kTransform, kTexture
        };

        rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	// CBVを使う
        rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;	// PixelShaderで使う
        rootParameters[0].Descriptor.ShaderRegister = 0;	// レジスタ番号0とバインド

        rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	// CBVを使う
        rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;	// VertexShaderで使う
        rootParameters[1].Descriptor.ShaderRegister = 0;	// レジスタ番号0とバインド

        rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // DescriptorTableを使う
        rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
        rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;	// Tableの中身の配列を指定
        rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);	// Tableで利用する数

        // シリアライズ
        static D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
        descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
        descriptionRootSignature.pParameters = rootParameters.data();
        descriptionRootSignature.NumParameters = (UINT)rootParameters.size();
        descriptionRootSignature.pStaticSamplers = staticSamplerDescs.data();
        descriptionRootSignature.NumStaticSamplers = (UINT)staticSamplerDescs.size();


        Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob;
        Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

        HRESULT hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
        if (FAILED(hr)) {
            Logger::Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
            assert(false);
        }

        Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
        hr = DirectXBase::GetInstance()->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
        assert(SUCCEEDED(hr));

        return rootSignature;
        };

    config.inputLayoutGenerator = [](){
        return std::vector<D3D12_INPUT_ELEMENT_DESC>
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        };
        };

    // 深度設定
    config.depthEnable = false;
    config.depthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;


    PSOManager::GetInstance()->RegisterPSOConfig(psoName_, config);
}



