#include "SkyBox.h"
#include "TextureManager.h"
#include "ModelManager.h" 
#include "Logger.h"
#include "SrvManager.h"
#include "Camera.h"

void SkyBox::Initialize()
{
	// PSOの設定
	PSOManager::PSOConfig config{};
	config.vertexShaderPath = L"resources/shaders/SkyBox.VS.hlsl";
	config.pixelShaderPath = L"resources/shaders/SkyBox.PS.hlsl";

	// RootSignatureの設定
	config.rootSignatureGenerator = [](){
		std::vector<D3D12_ROOT_PARAMETER> rootParameters;
		std::vector<D3D12_STATIC_SAMPLER_DESC> staticSamplerDescs;
		D3D12_STATIC_SAMPLER_DESC sampler{};
		sampler = PSOManager::GetInstance()->GetDefaultStaticSamplerDesc();

		staticSamplerDescs.push_back(sampler);
		D3D12_DESCRIPTOR_RANGE descriptorRange[1]{};
		descriptorRange[0].BaseShaderRegister = 0; // t0
		descriptorRange[0].NumDescriptors = 1;
		descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		rootParameters.resize(3);

		// Enum定義 (可読性のため)
		enum {
			kMaterial, kTransform, kTexture, DirLight, PointLight, SpotLight, Count, kCamera
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

		//rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	// CBVを使う
		//rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;	// PixelShaderで使う
		//rootParameters[3].Descriptor.ShaderRegister = 1;	// レジスタ番号1を使う

		//rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	// CBVを使う
		//rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;	// PixelShaderで使う
		//rootParameters[4].Descriptor.ShaderRegister = 2;	// レジスタ番号2を使う





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
			//{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			//{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};
		};

	// 深度設定
	config.depthEnable = true;
	config.depthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	config.depthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	PSOManager::GetInstance()->RegisterPSOConfig(psoName_, config);

	modelData_ = CreateSkyBox();

	TextureManager::GetInstance()->LoadTexture("resources/output_skybox.dds");
	modelData_.material.textureFilePath = "resources/output_skybox.dds";
	// テクスチャファイル読み込み
	TextureManager::GetInstance()->LoadTexture(modelData_.material.textureFilePath);
	// 読み込んだテクスチャの番号を取得
	modelData_.material.textureIndex =
		TextureManager::GetInstance()->GetTextureIndexByFilePath(modelData_.material.textureFilePath);

	

	CreateIndexResource();
	CreateTransformationMatrixResource();
	CreateVertexResource();
	CreateMaterialResource();
	
	transform_ = 
	{
		10.0f, 10.0f, 10.0f, // scale
		0.0f, 0.0f, 0.0f, // rotate
		0.0f, 0.0f, 0.0f, // translate
	};

	
}

void SkyBox::Update()
{
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	Matrix4x4 worldViewProjectionMatrix;
	if (camera_)
	{
		const Matrix4x4& viewProjectionMatrix = camera_->GetViewProjectionMatrix();
		worldViewProjectionMatrix = Multiply(worldMatrix, viewProjectionMatrix);
		
	} 
	else
	{
		worldViewProjectionMatrix = worldMatrix;
	}

	transformationMatrixData_->World = worldMatrix;
	transformationMatrixData_->WVP = worldViewProjectionMatrix;
}

void SkyBox::Draw()
{
	auto psoSet = PSOManager::GetInstance()->GetPSOData(psoName_, blendMode_, fillMode_);

	// パイプラインステートとルートシグネチャをセット
	DirectXBase::GetInstance()->GetCommandList()->SetPipelineState(psoSet.pipelineState.Get());
	DirectXBase::GetInstance()->GetCommandList()->SetGraphicsRootSignature(psoSet.rootSignature.Get());

	// 形状を設定。PS0に設定しているものとはまた別。同じものを設定すると考えておけば良い
	DirectXBase::GetInstance()->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	// wvp用のCBufferの場所を設定
	DirectXBase::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResource_->GetGPUVirtualAddress());
	// IndexBufferViewを設定
	DirectXBase::GetInstance()->GetCommandList()->IASetIndexBuffer(&indexBufferView);
	// VertexBufferViewを設定
	DirectXBase::GetInstance()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);	// VBVを設定
	// マテリアルのCBufferの場所を設定
	DirectXBase::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());

	/*TextureManager::GetInstance()->GetSRVHandleGPU(modelData_.material.textureFilePath);*/
	
	// テクスチャをセット
	SrvManager::GetInstance()->SetGraphicsRootDescriptorTable(2, modelData_.material.textureIndex);

	assert(modelData_.material.textureIndex != 0);

	// 描画
	DirectXBase::GetInstance()->GetCommandList()->DrawIndexedInstanced(36, 1, 0, 0, 0);

}

void SkyBox::Finalize()
{
	
}

void SkyBox::CreateIndexResource()
{
	// IndexBufferResourceを作成する。uint32_t 36個分のサイズを用意する
	indexResource_ = DirectXBase::GetInstance()->CreateBufferResource(sizeof(uint32_t) * 36);
	// IndexBufferResourceにデータを書き込むためのアドレスを取得してindexDataに割り当てる
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));

	// インデックスデータの値を書き込む
	indexData_[0] = 0; indexData_[1] = 1; indexData_[2] = 2;
	indexData_[3] = 2; indexData_[4] = 1; indexData_[5] = 3;

	indexData_[6] = 4; indexData_[7] = 5; indexData_[8] = 6;
	indexData_[9] = 6; indexData_[10] = 5; indexData_[11] = 7;

	indexData_[12] = 8; indexData_[13] = 9; indexData_[14] = 10;
	indexData_[15] = 10; indexData_[16] = 9; indexData_[17] = 11;

	indexData_[18] = 12; indexData_[19] = 13; indexData_[20] = 14;
	indexData_[21] = 14; indexData_[22] = 13; indexData_[23] = 15;

	indexData_[24] = 16; indexData_[25] = 17; indexData_[26] = 18;
	indexData_[27] = 18; indexData_[28] = 17; indexData_[29] = 19;

	indexData_[30] = 20; indexData_[31] = 21; indexData_[32] = 22;
	indexData_[33] = 22; indexData_[34] = 21; indexData_[35] = 23;

	// IndexBufferViewの内容を設定する
	indexBufferView.BufferLocation = indexResource_->GetGPUVirtualAddress();
	indexBufferView.SizeInBytes = sizeof(uint32_t) * 36;
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;
}

void SkyBox::CreateTransformationMatrixResource()
{
	// 座標変換行列リソースを作成する。Matrix4x4 1つ分のサイズを用意する
	transformationMatrixResource_ =DirectXBase::GetInstance()->CreateBufferResource(sizeof(TransformationMatrix));
	// TransformationMatrixResourceにデータを書き込むためのアドレスを取得してTransformationMatrixDataに割り当てる
	transformationMatrixResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData_));

	// 座標変換行列データの初期値を書き込む
	transformationMatrixData_->World = MakeIdentity4x4();
	transformationMatrixData_->WVP = MakeIdentity4x4();
}

void SkyBox::CreateVertexResource()
{
	// VertexResourceを作成する。
	vertexResource_ = DirectXBase::GetInstance()->
		CreateBufferResource(sizeof(Model::VertexData) * modelData_.vertices.size());

	assert(vertexResource_ && "CreateBufferResource failed");

	// VertexBufferViewの設定
	// リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点のサイズ
	vertexBufferView_.SizeInBytes = UINT(sizeof(Model::VertexData) * modelData_.vertices.size());
	// 1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(Model::VertexData);

	// VertexResourceにデータを書き込むためのアドレスを取得してVertexDataに割り当てる
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	// 頂点データをコピーする
	std::memcpy(vertexData_, modelData_.vertices.data(), sizeof(Model::VertexData) * modelData_.vertices.size());
}

void SkyBox::CreateMaterialResource()
{
	// マテリアルリソースを作成する。
	materialResource_ = DirectXBase::GetInstance()->CreateBufferResource(sizeof(Model::Material));
	// MaterialResourceにデータを書き込むためのアドレスを取得してMaterialDataに割り当てる
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));

	// 色の書き込み
	materialData_->color = modelData_.material.color;
	materialData_->enableLighting = true;
	materialData_->uvTransform = MakeIdentity4x4();
	materialData_->shininess = 10.0f;
}

Model::ModelData SkyBox::CreateSkyBox()
{
	// 1. 中で必要となる変数の宣言
	Model::ModelData mesh; //メッシュデータ



	std::vector<Model::VertexData> vertexData;
	vertexData.resize(24);

	

	// CubeのVaretxを作っておく(xが横、yが縦、zが奥行)
	std::vector<Vector4> cubeVertex =
	{
		//　正面から見て右側、XZ平面
		{  1.0f,  1.0f,  1.0f, 1.0f }, // 0
		{  1.0f,  1.0f, -1.0f, 1.0f }, // 1
		{  1.0f, -1.0f,  1.0f, 1.0f }, // 2
		{  1.0f, -1.0f, -1.0f, 1.0f }, // 3

		// 正面から見て左側、XZ平面
		{ -1.0f,  1.0f, -1.0f, 1.0f }, // 4
		{ -1.0f,  1.0f,  1.0f, 1.0f }, // 5
		{ -1.0f, -1.0f, -1.0f, 1.0f }, // 6
		{ -1.0f, -1.0f,  1.0f, 1.0f }, // 7
	};

	// 右面 (+X)
	vertexData[0].position = { 1,  1,  1, 1 };  // 右上奥
	vertexData[1].position = { 1,  1, -1, 1 };  // 右上手前
	vertexData[2].position = { 1, -1,  1, 1 };  // 右下奥
	vertexData[3].position = { 1, -1, -1, 1 };  // 右下手前

	// 左面 (-X)
	vertexData[4].position = { -1,  1, -1, 1 };  // 左上手前
	vertexData[5].position = { -1,  1,  1, 1 };  // 左上奥
	vertexData[6].position = { -1, -1, -1, 1 };  // 左下手前
	vertexData[7].position = { -1, -1,  1, 1 };  // 左下奥

	// 奥面 (+Z)
	vertexData[8].position = { -1,  1,  1, 1 };  // 左上
	vertexData[9].position = { 1,  1,  1, 1 };  // 右上
	vertexData[10].position = { -1, -1,  1, 1 };  // 左下
	vertexData[11].position = { 1, -1,  1, 1 };  // 右下

	// 手前面 (-Z)
	vertexData[12].position = { 1,  1, -1, 1 };  // 右上
	vertexData[13].position = { -1,  1, -1, 1 };  // 左上
	vertexData[14].position = { 1, -1, -1, 1 };  // 右下
	vertexData[15].position = { -1, -1, -1, 1 };  // 左下

	// 上面 (+Y)
	vertexData[16].position = { -1,  1, -1, 1 };  // 左手前
	vertexData[17].position = { 1,  1, -1, 1 };  // 右手前
	vertexData[18].position = { -1,  1,  1, 1 };  // 左奥
	vertexData[19].position = { 1,  1,  1, 1 };  // 右奥

	// 下面 (-Y)
	vertexData[20].position = { -1, -1,  1, 1 };  // 左奥
	vertexData[21].position = { 1, -1,  1, 1 };  // 右奥
	vertexData[22].position = { -1, -1, -1, 1 };  // 左手前
	vertexData[23].position = { 1, -1, -1, 1 };  // 右手前


	for (int i = 0; i < 24; i++)
	{
		vertexData[i].texcoord = { 0.0f, 0.0f };
		vertexData[i].normal = { 0.0f, 0.0f, 0.0f };
	}

	mesh.vertices = vertexData;

	mesh.material.textureFilePath;
	mesh.material.color = Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
	mesh.material.textureIndex;

	
	return mesh;
}
