#include "Enemy.h"
#include "Logger.h"
#include "TextureManager.h"
#include "SrvManager.h"
#include "CameraManager.h"
#include "BulletManager.h"
#include "DebugDrawManager.h"

void Enemy::Initialize()
{
	deltaTime_ = 1.0f / 60.0f; // 仮の値。実際のゲームループで更新されるべ

	PSOManager::GetInstance()->RegisterEnvironmentPSO();

	ModelManager::GetInstance()->LoadModel("", "axis.obj");

	object3d_ = std::make_unique<Object3d>();
	object3d_->Initialize();
	object3d_->SetModel("axis.obj");
	object3d_->GetModel()->SetEnvironmentCoefficient(0.2f, 0);

	collider_ = std::make_unique<Collider>();
	collider_->SetOwner(this);
	collider_->SetRadius(1.0f);
	collider_->SetAttribute(CollisionAttribute::Enemy);
	collider_->SetMask(CollisionAttribute::Enemy);

	transform_.scale = { 1.0f, 1.0f, 1.0f };
	transform_.rotate = { 0.0f, 0.0f, 0.0f };
	transform_.translate = { 0.0f, 0.0f, 30.0f };

	hitPoint_ = kMaxHitPoint;

	ChangeState(std::make_unique<EnemyIdleState>());
}

void Enemy::Update(const float& deltaTime)
{
	deltaTime_ = deltaTime;

	state_->Update(this, deltaTime);

	transform_.translate += velocity_ * deltaTime;

#ifdef _DEBUG
	ImGui::Begin("EnemySetting");
	bool isDraw = isDraw_;
	if (ImGui::Checkbox("DrawEnemy", &isDraw))
	{
		isDraw_ = isDraw;
	}

	ImGui::InputInt("HitPoint", &hitPoint_);

	ImGui::End();
#endif

	object3d_->SetTransform(transform_);
	object3d_->Update();

	if (damageTimer_ > 0.0f)
	{
		damageTimer_ -= kDeltaTime;
	}
	if (damageTimer_ < 0.0f)
	{
		damageTimer_ = 0.0f;
		// isPlayHitSE_ = false;
	}

	collider_->SetWorldPosition(GetWorldPosition());
}

void Enemy::Draw()
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
		collider_->GetRadius(), {1.0f, 0.0f, 1.0f, 1.0f}, 8);
}

void Enemy::Finalize()
{

}

void Enemy::ChangeState(std::unique_ptr<IEnemyState> newState)
{
	state_ = std::move(newState);
	state_->Initialize(this);
}

void Enemy::OnCollision(Collider* self, Collider* other)
{
	if (damageTimer_ == 0.0f)
	{
		hitPoint_--;
		damageTimer_ = kDamageInvincible_;
		//PlaySEHit();
	}
	if (hitPoint_ <= 0)
	{
		isDead_ = true;
		//PlaySEDead();
	}

	/*if (other->GetAttribute() == static_cast<uint32_t>(CollisionAttribute::Player))
	{
		
	}*/
}

void Enemy::Move(const float& directionX, const float& directionY)
{
	float targetVelocityX = maxSpeed_.x * directionX;
	float targetVelocityY = maxSpeed_.y * directionY;

	velocity_.x += (targetVelocityX - velocity_.x) * lerpFactor_;
	velocity_.y += (targetVelocityY - velocity_.y) * lerpFactor_;
}

void Enemy::Decelerate()
{
	velocity_.x += (0.0f - velocity_.x) * lerpFactor_;
	velocity_.y += (0.0f - velocity_.y) * lerpFactor_;
}

void Enemy::Shot()
{
	Vector3 bulletDirection = { 0.0f, 0.0f, -1.0f };
	bulletDirection = Normalize(TransformNormal(bulletDirection, object3d_->GetWorldTransform()->worldMatrix_));
	BulletManager::GetInstance()->CreateEnemyBullet(GetWorldPosition(), bulletDirection * bulletSpeed_);
	ChangeState(std::make_unique<EnemyShotState>());
}

void Enemy::Attack()
{}

const Vector3 Enemy::GetWorldPosition() const
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

const Vector3 Enemy::GetWorldRotate() const
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

void Enemy::SetParent(WorldTransform* worldTransform)
{
	object3d_->SetParent(worldTransform);
}
