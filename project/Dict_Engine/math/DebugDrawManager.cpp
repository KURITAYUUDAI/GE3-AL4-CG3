#define NOMINMAX
#include "DebugDrawManager.h"
#include <algorithm>
#include "Logger.h"

std::unique_ptr<DebugDrawManager> DebugDrawManager::instance_ = nullptr;

DebugDrawManager* DebugDrawManager::GetInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = std::make_unique<DebugDrawManager>(ConstructorKey());
	}
	return instance_.get();
}

void DebugDrawManager::Finalize()
{
	instance_.reset();
}

void DebugDrawManager::Initialize() 
{
   RegisterPSO();
   CreateVertexBufferResource();
   CreateTransformationResource();
}

void DebugDrawManager::AddLine(const Vector3& start, const Vector3& end, const Vector4& color) {
    if (vertices_.size() + 2 > maxVertices_) return;
    vertices_.push_back({ Vector4(start.x, start.y, start.z, 1.0f), color });
    vertices_.push_back({ Vector4(end.x, end.y, end.z, 1.0f), color });
}

void DebugDrawManager::AddBox(const Vector3& center, const Vector3& size, const Vector4& color) {
    Vector3 ext = { size.x * 0.5f, size.y * 0.5f, size.z * 0.5f };
    // 8つの頂点を計算
    Vector3 v[8] = {
        { center.x - ext.x, center.y - ext.y, center.z - ext.z },
        { center.x + ext.x, center.y - ext.y, center.z - ext.z },
        { center.x + ext.x, center.y + ext.y, center.z - ext.z },
        { center.x - ext.x, center.y + ext.y, center.z - ext.z },
        { center.x - ext.x, center.y - ext.y, center.z + ext.z },
        { center.x + ext.x, center.y - ext.y, center.z + ext.z },
        { center.x + ext.x, center.y + ext.y, center.z + ext.z },
        { center.x - ext.x, center.y + ext.y, center.z + ext.z }
    };
    // 12本の線分（計24頂点）として登録
    AddLine(v[0], v[1], color); AddLine(v[1], v[2], color); AddLine(v[2], v[3], color); AddLine(v[3], v[0], color); // 手前
    AddLine(v[4], v[5], color); AddLine(v[5], v[6], color); AddLine(v[6], v[7], color); AddLine(v[7], v[4], color); // 奥
    AddLine(v[0], v[4], color); AddLine(v[1], v[5], color); AddLine(v[2], v[6], color); AddLine(v[3], v[7], color); // 繋ぐ線
}

void DebugDrawManager::AddSphere(const Vector3& center, float radius, const Vector4& color, uint32_t segments) {
    // 緯線・経線をLINELISTで繋ぐ（簡易的に3軸の輪っかを描画するだけでもデバッグ用なら十分見やすいです）
    // ここでは一番手軽で視認性の高い「X, Y, Z軸に沿った3本の輪っか」を描画するアプローチを紹介します
    float angleStep = (2.0f * 3.14159265f) / segments;

    for (uint32_t i = 0; i < segments; ++i) {
        float a1 = i * angleStep;
        float a2 = (i + 1) * angleStep;

        // XY平面の輪
        AddLine(center + Vector3(cosf(a1) * radius, sinf(a1) * radius, 0), center + Vector3(cosf(a2) * radius, sinf(a2) * radius, 0), color);
        // XZ平面の輪
        AddLine(center + Vector3(cosf(a1) * radius, 0, sinf(a1) * radius), center + Vector3(cosf(a2) * radius, 0, sinf(a2) * radius), color);
        // YZ平面の輪
        AddLine(center + Vector3(0, cosf(a1) * radius, sinf(a1) * radius), center + Vector3(0, cosf(a2) * radius, sinf(a2) * radius), color);
    }
}

// --- 実際の描画処理 ---
void DebugDrawManager::DrawAll(const Matrix4x4& viewProjectionMatrix) 
{
    if (vertices_.empty()) 
    {
        return;
    }

    std::memcpy(vertexData_, vertices_.data(), sizeof(DebugVertex) * vertices_.size());
    *transformationData_ = viewProjectionMatrix;

    auto commandList = DirectXBase::GetInstance()->GetCommandList();
    auto& psoSet = PSOManager::GetInstance()->GetPSOData(psoName_, blendMode_, fillMode_);

    DirectXBase::GetInstance()->GetCommandList()->SetPipelineState(psoSet.pipelineState.Get());
    DirectXBase::GetInstance()->GetCommandList()->SetGraphicsRootSignature(psoSet.rootSignature.Get());

    DirectXBase::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(0, transformationResource_->GetGPUVirtualAddress());

    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
    commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
    commandList->DrawInstanced(static_cast<UINT>(vertices_.size()), 1, 0, 0);

    // 次のフレームのためにクリア
    vertices_.clear();
}

void DebugDrawManager::RegisterPSO()
{
	// PSOの設定
	PSOManager::PSOConfig config{};
	config.vertexShaderPath = L"resources/shaders/DebugDraw/DebugDraw.VS.hlsl";
	config.pixelShaderPath = L"resources/shaders/DebugDraw/DebugDraw.PS.hlsl";

	// RootSignatureの設定
	config.rootSignatureGenerator = []()
        {
		std::vector<D3D12_ROOT_PARAMETER> rootParameters;
		std::vector<D3D12_STATIC_SAMPLER_DESC> staticSamplerDescs;
		
		rootParameters.resize(1);

		// Enum定義 (可読性のため)
		enum {
			kTransform, 
		};

		rootParameters[kTransform].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	// CBVを使う
		rootParameters[kTransform].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;	// VertexShaderで使う
		rootParameters[kTransform].Descriptor.ShaderRegister = 0;	// レジスタ番号0とバインド

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
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};
		};

	// 深度設定
	config.depthEnable = true;
	config.depthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;

    config.toporogyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;

	PSOManager::GetInstance()->RegisterPSOConfig(psoName_, config);
}

void DebugDrawManager::CreateVertexBufferResource()
{
    // 頂点リソースを作成する。
    vertexResource_ = DirectXBase::GetInstance()->
        CreateBufferResource(sizeof(DebugVertex) * maxVertices_);
    // vertexDataにデータを書き込むためのアドレスを取得
    vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));   

    // VBVの設定
    vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
    vertexBufferView_.SizeInBytes = sizeof(DebugVertex) * maxVertices_;
    vertexBufferView_.StrideInBytes = sizeof(DebugVertex);

    vertices_.reserve(maxVertices_);
}

void DebugDrawManager::CreateTransformationResource()
{
    // 座標変換行列リソースを作成する。Matrix4x4 1つ分のサイズを用意する
    transformationResource_ = DirectXBase::GetInstance()->CreateConstantBufferResource(sizeof(Matrix4x4));
    // TransformationMatrixResourceにデータを書き込むためのアドレスを取得してTransformationMatrixDataに割り当てる
    transformationResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationData_));

    // 座標変換行列データの初期値を書き込む
    *transformationData_ = MakeIdentity4x4();
}
