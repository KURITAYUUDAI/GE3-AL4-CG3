#include "Bullet.h"
#include "assert.h"
#include "WorldTransformAssist.h"
#include "BulletManager.h"

void Bullet::Initialize(Model* model, const Camera* camera, const Vector3& position, 
	const Vector3& velocity, const ID& id) 
{
	assert(model);
	model_ = model;
	// テクスチャ読み込み
	textureHandle_ = TextureManager::Load("uvChecker.png");
	camera_ = camera;
	worldTransform_.Initialize();
	worldTransform_.scale_ = {0.5f, 0.5f, 0.5f};
	worldTransform_.rotation_ = {0.0f, 0.0f, 0.0f};
	worldTransform_.translation_ = position;
	velocity_ = velocity;
	id_ = id;
}

void Bullet::Update() {
	worldTransform_.translation_ += velocity_;
	worldTransform_.matWorld_ = BulletManager::GetInstance()->GetBillboardWorldMatrix(
		worldTransform_.scale_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();
	if (--deathTimer_ <= 0)
	{
		isDead_ = true;
	}
}

void Bullet::Draw()
{
	model_->Draw(worldTransform_, *camera_, nullptr); 
}

void Bullet::OnCollision(const Enemy* enemy)
{
	(void)enemy;
	isDead_ = true;
}

void Bullet::OnCollision(const Player* player)
{
	(void)player;
	isDead_ = true;
}

const Vector3 Bullet::GetWorldPosition() const 
{
	// ワールド座標を入れる変数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];
	return worldPos;
}

AABB Bullet::GetAABB() 
{
	Vector3 worldPos = GetWorldPosition();
	AABB aabb;
	aabb.min = { worldPos.x - size_.x / 2.0f, worldPos.y - size_.y / 2.0f, worldPos.z - size_.z / 2.0f };
	aabb.max = { worldPos.x + size_.x / 2.0f, worldPos.y + size_.y / 2.0f, worldPos.z + size_.z / 2.0f };
	return aabb;
}