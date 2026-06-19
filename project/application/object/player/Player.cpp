#define NOMINMAX
#include "Player.h"
#include "Logger.h"
#include "TextureManager.h"
#include "SrvManager.h"
#include "InputManager.h"
#include "BulletManager.h"
#include "CameraManager.h"
#include "DebugDrawManager.h"
#include "PlayerEvent.h"

void Player::Initialize()
{
	deltaTime_ = 1.0f / 60.0f; // 仮の値。実際のゲームループで更新されるべ

	PSOManager::GetInstance()->RegisterEnvironmentPSO();

	selector_.GetKeyboardHandler()->AssignKey("shot", DIK_SPACE);
	selector_.GetKeyboardHandler()->AssignKey("avoid", DIK_E);

	selector_.GetGamepadHandler()->AssignKey("shot", XINPUT_GAMEPAD_RIGHT_SHOULDER);
	selector_.GetGamepadHandler()->AssignKey("avoid", XINPUT_GAMEPAD_X);

	ModelManager::GetInstance()->LoadModel("", "sphere.obj");

	object3d_ = std::make_unique<Object3d>();
	object3d_->Initialize();
	object3d_->SetModel("sphere.obj");

	collider_ = std::make_unique<Collider>();
	collider_->SetOwner(this);
	collider_->SetRadius(1.0f);
	collider_->SetAttribute(CollisionAttribute::Player);
	collider_->SetMask(CollisionAttribute::Player);

	transform_.scale = { 1.0f, 1.0f, 1.0f };
	transform_.rotate = { 0.0f, 0.0f, 0.0f };
	transform_.translate = { 0.0f, 0.0f, 15.0f };

	hitPoint_ = kMaxHitPoint;

	ChangeState(std::make_unique<PlayerIdleState>());
}

void Player::EventDispatch()
{
	eventBus_->Publish(PlayerHPChangeEvent
		{
			.currentHitPoint = hitPoint_,
			.previousHitPoint = hitPoint_,
			.maxHitPoint = kMaxHitPoint
		}
	);

	eventBus_->Dispatch();
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
	Vector3 worldPosition = GetWorldPosition();
	ImGui::InputFloat3 ("WorldPosition", &worldPosition.x, "%.3f", ImGuiInputTextFlags_ReadOnly);
	Vector3 worldRotate = GetWorldRotate();
	ImGui::InputFloat3 ("WorldRotate", &worldRotate.x, "%.3f", ImGuiInputTextFlags_ReadOnly);

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

	CameraManager::GetInstance()->LimitPlayerInFrustum(transform_.translate);

	object3d_->SetTransform(transform_);
	object3d_->Update();

	if (damageTimer_ > 0.0f)
	{
		damageTimer_ -= kDeltaTime;
	}
	if (damageTimer_ < 0.0f)
	{
		damageTimer_ = 0.0f;
		//isPlayHitSE_ = false;
	}

	collider_->SetWorldPosition(GetWorldPosition());
}

void Player::Draw()
{
	object3d_->SetPsoName(psoName_);
	object3d_->SetBlendMode(blendMode_);
	object3d_->SetFillMode(fillMode_);
	auto psoSet = PSOManager::GetInstance()->GetPSOData(psoName_, blendMode_, fillMode_);

	// パイプラインステートとルートシグネチャをセット
	DirectXBase::GetInstance()->GetCommandList()->SetPipelineState(psoSet.pipelineState.Get());
	DirectXBase::GetInstance()->GetCommandList()->SetGraphicsRootSignature(psoSet.rootSignature.Get());

	// 形状を設定。PS0に設定しているものとはまた別。同じものを設定すると考えておけば良い
	DirectXBase::GetInstance()->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	SrvManager::GetInstance()->SetGraphicsRootDescriptorTable(5, environmentTextureIndex_);

	if (isDraw_)
	{
		if (static_cast<int>(damageTimer_ * 60.0f) % 5 == 0)
		{
			object3d_->Draw();
		}
	}

	DebugDrawManager::GetInstance()->AddSphere(GetWorldPosition(),
		collider_->GetRadius(), { 1.0f, 1.0f, 1.0f, 1.0f }, 8);
}

void Player::Finalize()
{

}

void Player::ChangeState(std::unique_ptr<IPlayerState> newState)
{
	state_ = std::move(newState);
	state_->Initialize(this);
}

void Player::OnCollision(Collider* self, Collider* other)
{
	if (damageTimer_ == 0.0f)
	{
		Damage(1);
		damageTimer_ = kDamageInvincible_;
		//PlaySEHit();
	}
	if (hitPoint_ <= 0)
	{
		isDead_ = true;
		//PlaySEDead();
	}

	/*if (other->GetAttribute() == static_cast<uint32_t>(CollisionAttribute::Enemy))
	{
		
	}*/
}

void Player::Damage(int damage)
{
	const int previousHP = hitPoint_;

	hitPoint_ -= damage;
	if (hitPoint_ < 0)
	{
		hitPoint_ = 0;
	}

	if (hitPoint_ == previousHP)
	{
		return;
	}
	if (eventBus_)
	{
		eventBus_->Publish(PlayerHPChangeEvent
			{
				.currentHitPoint = hitPoint_,
				.previousHitPoint = previousHP,
				.maxHitPoint = kMaxHitPoint
			}
		);
	}
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

