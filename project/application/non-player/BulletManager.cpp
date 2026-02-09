#include "BulletManager.h"
#include "Bullet.h"

BulletManager* BulletManager::instance_ = nullptr;

BulletManager* BulletManager::GetInstance() 
{
	if (instance_ == nullptr) 
	{
		instance_ = new BulletManager;
	}
	return instance_;
}

void BulletManager::Finalize() 
{
	for (auto bullet : bullets_) 
	{
		delete bullet;
	}
	bullets_.clear();
	delete instance_;
	instance_ = nullptr;
}

void BulletManager::Initialize(Model* playerBulletModel, Model* enemyBulletModel, Camera* camera) 
{
	modelPlayerBullet_ = playerBulletModel;
	modelEnemyBullet_ = enemyBulletModel;
	camera_ = camera;

	shotSEDataHandle_ = Audio::GetInstance()->LoadWave("SE/shotBullet.wav");
}

void BulletManager::Update() {
	Matrix4x4 backToFrontMatrix = MakeRotateYMatrix(pi);
	billboardMatrix_ = Multiply(backToFrontMatrix, Inverse(camera_->matView));
	billboardMatrix_.m[3][0] = 0.0f;
	billboardMatrix_.m[3][1] = 0.0f;
	billboardMatrix_.m[3][2] = 0.0f;

	for (auto it = bullets_.begin(); it != bullets_.end();) 
	{
		Bullet* bullet = *it;
		bullet->Update();
		if (bullet->GetIsDead()) 
		{
			delete bullet;
			it = bullets_.erase(it);
		}
		else 
		{
			++it;
		}
	}
}

void BulletManager::Draw() 
{
	for (auto& bullet : bullets_) 
	{
		bullet->Draw();
	}
}

void BulletManager::CreatePlayerBullet(const Vector3& position, const Vector3& velocity) 
{
	if (bullets_.size() >= kMaxBullet) 
	{
		return;
	}
	Bullet* bullet = new Bullet;
	bullet->Initialize(modelPlayerBullet_, camera_, position, velocity, Bullet::ID::kPlayer);
	bullets_.push_back(bullet);

	PlaySEShot();
	isPlayShotSE_ = false;
}

void BulletManager::CreateEnemyBullet(const Vector3& position, const Vector3& velocity) 
{
	if (bullets_.size() >= kMaxBullet) 
	{
		return;
	}
	Bullet* bullet = new Bullet;
	bullet->Initialize(modelEnemyBullet_, camera_, position, velocity, Bullet::ID::kEnemy);
	bullets_.push_back(bullet);

	PlaySEShot();
	isPlayShotSE_ = false;
}

const Matrix4x4 BulletManager::GetBillboardWorldMatrix(const Vector3& scale, const Vector3& translate) const
{
	Matrix4x4 scaleMatrix = MakeScaleMatrix(scale);
	Matrix4x4 translateMatrix = MakeTranslateMatrix(translate);

	Matrix4x4 worldMatrix = scaleMatrix * billboardMatrix_ * translateMatrix;

	return worldMatrix;
}

void BulletManager::PlaySEShot()
{
	if (!isPlayShotSE_)
	{
		shotSEHandle_ = Audio::GetInstance()->PlayWave(shotSEDataHandle_, false, 0.1f);
		isPlayShotSE_ = true;
	}
}