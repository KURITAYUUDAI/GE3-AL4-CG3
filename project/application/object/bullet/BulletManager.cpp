#include "BulletManager.h"
#include "ModelManager.h"
#include "Model.h"

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
	ModelManager::GetInstance()->LoadModel("playerBullet.obj");
	ModelManager::GetInstance()->LoadModel("enemyBullet.obj");
}

void BulletManager::Update() 
{
	for (auto& bullet : bullets_)
	{
		bullet->Update();
	}

	bullets_.remove_if([](const std::unique_ptr<Bullet>& bullet){
		return bullet->GetIsDead();
	});
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
	std::unique_ptr<Bullet> newBullet = std::make_unique<Bullet>();
	newBullet->Initialize(position, velocity, Bullet::ID::kPlayer, "playerBullet.obj");
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
	newBullet->Initialize(position, velocity, Bullet::ID::kEnemy, "enemyBullet.obj");
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