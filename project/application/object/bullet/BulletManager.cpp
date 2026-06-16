#include "BulletManager.h"
#include "ModelManager.h"
#include "Model.h"
#include "ImGuiManager.h"
#include "Logger.h"

std::unique_ptr<BulletManager> BulletManager::instance_ = nullptr;

BulletManager* BulletManager::GetInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = std::make_unique<BulletManager>(ConstructorKey());
	}
	return instance_.get();
}

void BulletManager::Finalize()
{
	bullets_.clear();
	instance_.reset();
}

void BulletManager::Initialize()
{
	// PSOの設定
	PSOManager::PSOConfig config{};
	config.vertexShaderPath = L"resources/shaders/Object3d/Object3d.VS.hlsl";
	config.pixelShaderPath = L"resources/shaders/Object3d/Object3d.PS.hlsl";

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


		D3D12_DESCRIPTOR_RANGE descriptorRangeE[1]{};
		descriptorRangeE[0].BaseShaderRegister = 1; // t1
		descriptorRangeE[0].NumDescriptors = 1;
		descriptorRangeE[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		descriptorRangeE[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		rootParameters.resize(5);

		// Enum定義 (可読性のため)
		enum {
			kMaterial, kTransform, kTexture, kLight, kCamera, kEnvironmentTexture
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

		rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	// CBVを使う
		rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;	// PixelShaderで使う
		rootParameters[3].Descriptor.ShaderRegister = 1;	// レジスタ番号1を使う

		rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	// CBVを使う
		rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;	// PixelShaderで使う
		rootParameters[4].Descriptor.ShaderRegister = 2;	// レジスタ番号2を使う

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
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};
		};

	// 深度設定
	config.depthEnable = true;
	config.depthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;

	PSOManager::GetInstance()->RegisterPSOConfig(psoName_, config);

	ModelManager::GetInstance()->LoadModel("playerBullet/playerBullet.obj");
	ModelManager::GetInstance()->LoadModel("enemyBullet/enemyBullet.obj");
}

void BulletManager::Update(const float& deltaTime)
{
	ImGui::Begin("Bullet Manager");

	// 現在の弾の総数を表示
	ImGui::Text("Active Bullets: %d / %d", bullets_.size(), kMaxBullet);
	ImGui::Separator();

	int index = 0;
	for (auto& bullet : bullets_)
	{
		// 弾の種類（Player / Enemy）を判定
		std::string bulletType = (bullet->GetID() == Bullet::ID::kPlayer) ? "Player" : "Enemy";

		// 折りたたみヘッダー（クリックで開閉）
		std::string label = "Bullet [" + std::to_string(index) + "] (" + bulletType + ")";
		if (ImGui::CollapsingHeader(label.c_str()))
		{
			// 座標と回転の取得
			Vector3 translation = bullet->GetTranslation();
			Vector3 rotation = bullet->GetRotation();

			// Text で現在の値を表示するだけ（見るだけ！）
			ImGui::Text("Translate : X:%.2f, Y:%.2f, Z:%.2f", translation.x, translation.y, translation.z);
			ImGui::Text("Rotate    : X:%.2f, Y:%.2f, Z:%.2f", rotation.x, rotation.y, rotation.z);

			// 速度もついでに見られるように
			Vector3 velocity = bullet->GetVelocity();
			ImGui::Text("Velocity  : X:%.2f, Y:%.2f, Z:%.2f", velocity.x, velocity.y, velocity.z);

			// デスタイマーも
			int32_t deathTimer = bullet->GetDeathTimer();
			ImGui::Text("DeathTimer  : %d", deathTimer);
		}

		index++;
	}

	ImGui::End();

	for (auto& bullet : bullets_)
	{

		bullet->Update(deltaTime);
	}

	bullets_.remove_if([](const std::unique_ptr<Bullet>& bullet){
		return bullet->GetIsDead();
	});
}

void BulletManager::Draw()
{
	auto psoSet = PSOManager::GetInstance()->GetPSOData(psoName_, blendMode_, fillMode_);

	// パイプラインステートとルートシグネチャをセット
	DirectXBase::GetInstance()->GetCommandList()->SetPipelineState(psoSet.pipelineState.Get());
	DirectXBase::GetInstance()->GetCommandList()->SetGraphicsRootSignature(psoSet.rootSignature.Get());

	// 形状を設定。PS0に設定しているものとはまた別。同じものを設定すると考えておけば良い
	DirectXBase::GetInstance()->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (auto& bullet : bullets_)
	{
		bullet->SetPsoName(psoName_);
		bullet->Draw();
	}
}

void BulletManager::CreatePlayerBullet(const Vector3& position, const Vector3& velocity)
{
	if (bullets_.size() >= kMaxBullet)
	{
		return;
	}
	std::unique_ptr<Bullet> newBullet = std::make_unique<Bullet>();
	newBullet->Initialize(position, velocity, Bullet::ID::kPlayer, "playerBullet/playerBullet.obj");
	bullets_.push_back(std::move(newBullet));

	/*PlaySEShot();
	isPlayShotSE_ = false;
	*/
}

void BulletManager::CreateEnemyBullet(const Vector3& position, const Vector3& velocity)
{
	if (bullets_.size() >= kMaxBullet)
	{
		return;
	}
	std::unique_ptr<Bullet> newBullet = std::make_unique<Bullet>();
	newBullet->Initialize(position, velocity, Bullet::ID::kEnemy, "enemyBullet/enemyBullet.obj");
	bullets_.push_back(std::move(newBullet));

	/*PlaySEShot();
	isPlayShotSE_ = false;
	*/
}



//void BulletManager::PlaySEShot()
//{
//	if (!isPlayShotSE_)
//	{
//		shotSEHandle_ = Audio::GetInstance()->PlayWave(shotSEDataHandle_, false, 0.1f);
//		isPlayShotSE_ = true;
//	}
//}