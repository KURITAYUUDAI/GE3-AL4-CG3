#include "Bullet.h"
#include "assert.h"
#include "BulletManager.h"
#include "ModelManager.h"
#include "CameraManager.h"
#include "Model.h"

//void Bullet::Initialize(Model* model, const Camera* camera, const Vector3& position,
//	const Vector3& velocity, const ID& id)
//{
//	assert(model);
//	model_ = model;
//	// テクスチャ読み込み
//	textureHandle_ = TextureManager::Load("uvChecker.png");
//	camera_ = camera;
//	transform_.Initialize();
//	transform_.scale_ = { 0.5f, 0.5f, 0.5f };
//	transform_.rotate_ = { 0.0f, 0.0f, 0.0f };
//	transform_.translate_ = position;
//	velocity_ = velocity;
//	id_ = id;
//}

void Bullet::Initialize(const Vector3& position, const Vector3& velocity, const ID& id, const std::string& modelName)
{
	deltaTime_ = 1.0f / 60.0f; // 仮の値。実際のゲームループで更新されるべ

	object3d_ = std::make_unique<Object3d>();
	object3d_->Initialize();
	object3d_->SetModel(modelName);

	transform_.scale = { 1.0f, 1.0f, 1.0f };
	transform_.rotate = { 0.0f, 0.0f, 0.0f };
	transform_.translate = position;
	velocity_ = velocity;

	id_ = id;
}

void Bullet::Update()
{
	transform_.translate += velocity_;
	
	Matrix4x4 billboard = CameraManager::GetInstance()->GetMainCamera()->
		GetBillboardWorldMatrix(transform_.scale, transform_.rotate, transform_.translate);
	
	object3d_->Update(&billboard);
	
	if (--deathTimer_ <= 0)
	{
		isDead_ = true;
	}
}

void Bullet::Draw()
{
	object3d_->Draw();
}

void Bullet::Finalize()
{
	
}

//void Bullet::OnCollision(const Enemy* enemy)
//{
//	(void)enemy;
//	isDead_ = true;
//}

void Bullet::OnCollision(const Player* player)
{
	(void)player;
	isDead_ = true;
}

const Vector3 Bullet::GetWorldPosition() const
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

AABB Bullet::GetAABB()
{
	Vector3 worldPos = GetWorldPosition();
	AABB aabb;
	aabb.min = { worldPos.x - size_.x / 2.0f, worldPos.y - size_.y / 2.0f, worldPos.z - size_.z / 2.0f };
	aabb.max = { worldPos.x + size_.x / 2.0f, worldPos.y + size_.y / 2.0f, worldPos.z + size_.z / 2.0f };
	return aabb;
}