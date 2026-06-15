#include "BulletManager.h"
#include "ModelManager.h"
#include "Model.h"
#include "ImGuiManager.h"

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