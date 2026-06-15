#include "Player.h"
#include "Logger.h"
#include "TextureManager.h"
#include "SrvManager.h"
#include "InputManager.h"
#include "BulletManager.h"

void Player::Initialize()
{
	deltaTime_ = 1.0f / 60.0f; // 仮の値。実際のゲームループで更新されるべ

	// PSOの設定
	PSOManager::PSOConfig config{};
	config.vertexShaderPath = L"resources/shaders/Environment.VS.hlsl";
	config.pixelShaderPath = L"resources/shaders/Environment.PS.hlsl";

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

		rootParameters.resize(6);

		// Enum定義 (可読性のため)
		enum {
			kMaterial, kTransform, kTexture, kLight, kCamera, kEnvironment
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

		rootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // DescriptorTableを使う
		rootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
		rootParameters[5].DescriptorTable.pDescriptorRanges = descriptorRangeE;	// Tableの中身の配列を指定
		rootParameters[5].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeE);	// Tableで利用する数



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

	selector_.GetKeyboardHandler()->AssignKey("shot", DIK_SPACE);
	selector_.GetKeyboardHandler()->AssignKey("avoid", DIK_E);

	selector_.GetGamepadHandler()->AssignKey("shot", XINPUT_GAMEPAD_RIGHT_SHOULDER);
	selector_.GetGamepadHandler()->AssignKey("avoid", XINPUT_GAMEPAD_X);

	ModelManager::GetInstance()->LoadModel("sphere.obj");

	object3d_ = std::make_unique<Object3d>();
	object3d_->Initialize();
	object3d_->SetModel("sphere.obj");

	transform_.scale = { 1.0f, 1.0f, 1.0f };
	transform_.rotate = { 0.0f, 0.0f, 0.0f };
	transform_.translate = { 0.0f, 0.0f, 15.0f };

	ChangeState(std::make_unique<PlayerIdleState>());
}

void Player::Update(const float& deltaTime)
{
	deltaTime_ = deltaTime;

	state_->Update(this, deltaTime);

	transform_.translate += velocity_ * deltaTime;
	/*transform_.rotate += (targetRoll_ - transform_.rotate) * 0.1f;*/

	

#ifdef _DEBUG
	ImGui::Begin("PlayerSetting");
	bool isDraw = isDraw_;
	if (ImGui::Checkbox("DrawPlayer", &isDraw))
	{
		isDraw_ = isDraw;
	}

	Transform transform = transform_;
	if (ImGui::DragFloat3("Scale", &transform.scale.x, 0.1f))
	{
		transform_ = transform;
	}
	if (ImGui::DragFloat3("Rotate", &transform.rotate.x, 0.1f))
	{
		transform_ = transform;
	}
	if (ImGui::DragFloat3("Translate", &transform.translate.x, 0.1f))
	{
		transform_ = transform;
	}
	ImGui::InputFloat3 ("Velocity", &velocity_.x, "%.3f", ImGuiInputTextFlags_ReadOnly);


	float environmentCoefficient = object3d_->GetModel()->GetEnvironmentCoefficient(0);
	if (ImGui::SliderFloat("Environment Coefficient", &environmentCoefficient, 0.0f, 1.0f))
	{
		object3d_->GetModel()->SetEnvironmentCoefficient(environmentCoefficient, 0);
	}

	ImGui::Text("PlayerState: %s", typeid(*state_).name());

	/*Vector2 leftStick =
	{
		InputManager::GetInstance()->GetLeftStickX(),
		InputManager::GetInstance()->GetLeftStickY()
	};
	ImGui::InputFloat2("Left Stick", &leftStick.x, "%.3f", ImGuiInputTextFlags_ReadOnly);

	bool isAPressed = InputManager::GetInstance()->PushButton(XINPUT_GAMEPAD_A);
	ImGui::Text("A Button: %s", isAPressed ? "Pressed" : "Released");
	
	float triggerRight = InputManager::GetInstance()->GetRightTrigger();
	ImGui::InputFloat("Right Trigger", &triggerRight, 0.1f, 0.1f, "%.3f", ImGuiInputTextFlags_ReadOnly);*/

	ImGui::End();
#endif

	object3d_->SetTransform(transform_);
	object3d_->Update();

}

void Player::Draw()
{
	object3d_->SetPsoName(psoName_);
	auto psoSet = PSOManager::GetInstance()->GetPSOData(psoName_, blendMode_, fillMode_);

	// パイプラインステートとルートシグネチャをセット
	DirectXBase::GetInstance()->GetCommandList()->SetPipelineState(psoSet.pipelineState.Get());
	DirectXBase::GetInstance()->GetCommandList()->SetGraphicsRootSignature(psoSet.rootSignature.Get());

	// 形状を設定。PS0に設定しているものとはまた別。同じものを設定すると考えておけば良い
	DirectXBase::GetInstance()->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	SrvManager::GetInstance()->SetGraphicsRootDescriptorTable(5, environmentTextureIndex_);

	if (isDraw_)
	{
		object3d_->Draw();
	}
}

void Player::Finalize()
{

}

void Player::ChangeState(std::unique_ptr<IPlayerState> newState)
{
	state_ = std::move(newState);
	state_->Initialize(this);
}

void Player::MoveHorizontal(const float& directionX, const float& directionY)
{
	float targetVelocityX = maxSpeed_.x * directionX;
	float targetVelocityY = maxSpeed_.y * directionY;

	// 現在速度を目標速度へ線形補間
	// → スティックの倒し具合に応じた速度に滑らかに追従する
	velocity_.x += (targetVelocityX - velocity_.x) * lerpFactor_;
	velocity_.y += (targetVelocityY - velocity_.y) * lerpFactor_;
}

void Player::Decelerate()
{
	velocity_.x += (0.0f - velocity_.x) * lerpFactor_;
	velocity_.y += (0.0f - velocity_.y) * lerpFactor_;
}

void Player::Shot()
{
	Vector3 bulletDirection = { 0.0f, 0.0f, 1.0f };
	bulletDirection = Normalize(TransformNormal(bulletDirection, object3d_->GetWorldTransform()->worldMatrix_));
	BulletManager::GetInstance()->CreatePlayerBullet(GetWorldPosition(), bulletDirection * bulletSpeed_);
	ChangeState(std::make_unique<PlayerShotState>());
}

void Player::Avoid(const Vector2& direction)
{
	ChangeState(std::make_unique<PlayerAvoidState>(direction));
}

void Player::MoveAvoid(const Vector3 direction, float speed)
{
	velocity_ = direction * speed;
	
	// 回避方向に回転する
	
}

void Player::SetTargetRoll(const Vector3 rollRadian)
{
	targetRoll_ = rollRadian;
}

const Vector3 Player::GetWorldPosition() const
{
	Matrix4x4 worldMatrix = object3d_->GetWorldTransform()->worldMatrix_;

	// ワールド座標を入れる変数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldMatrix.m[3][0];
	worldPos.y = worldMatrix.m[3][1];
	worldPos.z = worldMatrix.m[3][2];

	return worldPos;
}

const Vector3 Player::GetWorldRotate() const
{
	Matrix4x4 worldMatrix = object3d_->GetWorldTransform()->worldMatrix_;
	if (object3d_->GetWorldTransform()->parent_)
	{
		worldMatrix *= object3d_->GetWorldTransform()->parent_->worldMatrix_;
	}

	Vector3 worldRotEuler;
	// 行列からエウラー角を計算する（一般的な公式に基づく抽出）
	// ※アークサイン等を使うため、XYZの回転順序に依存します
	worldRotEuler.y = std::asin(-worldMatrix.m[0][2]);
	if (std::cos(worldRotEuler.y) > 0.0001f) {
		worldRotEuler.x = std::atan2(worldMatrix.m[1][2], worldMatrix.m[2][2]);
		worldRotEuler.z = std::atan2(worldMatrix.m[0][1], worldMatrix.m[0][0]);
	} else {
		worldRotEuler.x = std::atan2(-worldMatrix.m[2][1], worldMatrix.m[1][1]);
		worldRotEuler.z = 0.0f;
	}

	return worldRotEuler;
}

void Player::SetParent(WorldTransform* worldTransform)
{
	object3d_->SetParent(worldTransform);
}

