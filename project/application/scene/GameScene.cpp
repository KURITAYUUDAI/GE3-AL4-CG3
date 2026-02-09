#define NOMINMAX
#include "GameScene.h"
#include <limits>
#include "3d/AxisIndicator.h"

#include "Anchor.h"
#include "SeedManager.h"
#include "Bullet.h"
#include "BulletManager.h"
#include "SpriteDraw.h"
#include "StageTime.h"

using namespace KamataEngine;

GameScene::~GameScene() 
{
	delete player_;

	for (Enemy* enemy : enemies_)
	{
		delete enemy;
	}
	enemies_.clear();

	delete skydome_;

	BulletManager::GetInstance()->Finalize();
	
	delete deathParticles_;
	
	delete modelPlayer_;
	delete modelPlayerBullet_;
	delete modelAnchor_;

	delete modelEnemy_;
	delete modelEnemyBullet_;

	delete modelSkydome_;
	delete modelDeathParticle_;
	delete modelBackGround_;

	/*delete EX1Sprite_;
	delete EX2Sprite_;*/

	delete HowToOperate1Sprite_;
	delete HowToOperate2Sprite_;
	delete HowToOperate3Sprite_;

	for (Sprite* sprite : hitPointSprites_)
	{
		delete sprite;
	}
	hitPointSprites_.clear();

	delete this->debugCamera_;
}

void GameScene::Initialize()
{
	// メンバ変数への代入処理

	SeedManager::GetInstance()->Initialize();

	// テクスチャの読み込み
	textureHandle_ = TextureManager::Load("block.png");

	// 3Dモデルの生成
	modelPlayer_ = Model::CreateFromOBJ("player", true);

	modelPlayerBullet_ = Model::CreateFromOBJ("playerBullet", true);

	modelAnchor_ = Model::CreateFromOBJ("anchor", true);

	modelEnemy_ = Model::CreateFromOBJ("enemy", true);

	modelEnemyBullet_ = Model::CreateFromOBJ("enemyBullet", true);
	
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);
	modelDeathParticle_ = Model::CreateFromOBJ("deathParticles", true);

	modelBackGround_ = Model::CreateFromOBJ("backGround", true);

	// 天球の生成
	skydome_ = new Skydome();
	// 天球の初期化
	skydome_->Initialize(modelSkydome_, &camera_);

	

	// 自キャラの生成
	player_ = new Player();

	// 座標をマップチップ番号で指定
	Vector3 playerPosition = { 0.0f, -4.0f, 0.0f };

	// 自キャラの初期化
	player_->Initialize(modelPlayer_, &camera_, playerPosition, modelAnchor_);

	hitPointSprites_.clear();
	for (int i = 0; i < player_->GetHitPoint(); ++i)
	{
		Sprite* hitPointSprite = Sprite::Create(hitPointHandle, {0.0f, 0.0f});
		hitPointSprites_.push_back(hitPointSprite);
	}

	StageTime::GetInstance()->Initialize();

	respawnTimers_.clear();
	respawnTimers_.push_back(2.0f);

	//// 敵キャラの生成
	//for (int i = 0; i < 1; ++i)
	//{
	//	Enemy* enemy = new Enemy();

	//	Vector3 pos = 
	//	{
	//		SeedManager::GetInstance()->GenerateFloat(-5.0f, 5.0f), 
	//		SeedManager::GetInstance()->GenerateFloat(-5.0f, 5.0f), 
	//		SeedManager::GetInstance()->GenerateFloat(15.0f, 35.0f)
	//	};

	//	enemy->Initialize(modelEnemy_, &camera_, pos);

	//	enemies_.push_back(enemy);
	//}

	BulletManager::GetInstance()->Initialize(modelPlayerBullet_, modelEnemyBullet_, &camera_);

	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);

	// カメラの初期化
	camera_.farZ = 1000.0f;
	camera_.Initialize();
	

	// デバッグカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);

	// カメラコントローラーの初期化
	cameraController_ = new CameraController();
	cameraController_->Initialize();
	cameraController_->SetTarget(player_->GetWorldTransform().translation_);
	cameraController_->Reset();

	cameraController_->SetMovableArea({-100.0f, -100.0f, -100.0f}, {100.0f,100.0f, 100.0f});

	// 軸方向表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	// 軸方向表示が参照するカメラを指定する
	AxisIndicator::GetInstance()->SetTargetCamera(&camera_);

	phase_ = Phase::kFadeIn;

}

void GameScene::Update()
{
	// インゲームの更新処理を書く

	switch (phase_)
	{ 
	
	case Phase::kFadeIn:

		// 天球の更新
		skydome_->Update();

		//// 背景の更新
		//backGround_->Update();

		// 自キャラの更新
		player_->Update();

		for (Enemy* enemy : enemies_)
		{
			enemy->Update();
		}

		enemies_.remove_if([](Enemy* enemy)
		{
			if (enemy->GetIsDead())
			{
				delete enemy;
				StageTime::GetInstance()->AddScore();
				return true;
			}
			return false;
		});

		BulletManager::GetInstance()->Update();

		// カメラの処理
		if (isDebugCameraActive_) 
		{
			debugCamera_->Update();
			camera_.matView = debugCamera_->GetCamera().matView;
			camera_.matProjection = debugCamera_->GetCamera().matProjection;
			// ビュープロジェクション行列の転送
			camera_.TransferMatrix();
		} 
		else 
		{
			cameraController_->Update();
			camera_.matView = cameraController_->GetCamera().matView;
			camera_.matProjection = cameraController_->GetCamera().matProjection;
			camera_.TransferMatrix();
		}

		

		fade_->Update();

		if (fade_->IsFinished()) 
		{
			phase_ = Phase::kPlay;
		}

		break;

	case Phase::kPlay:

		// 天球の更新
		skydome_->Update();

		//// 背景の更新
		//backGround_->Update();

		// 自キャラの更新
		player_->Update();

		//if (Input::GetInstance()->TriggerKey(DIK_F4)) 
		//{
		//	Enemy* enemy = new Enemy();

		//	Vector3 pos = 
		//	{
		//		SeedManager::GetInstance()->GenerateFloat(-5.0f, 5.0f), 
		//		SeedManager::GetInstance()->GenerateFloat(-5.0f, 5.0f), 
		//		7.0f
		//	};

		//	enemy->Initialize(modelEnemy_, &camera_, pos);

		//	enemies_.push_back(enemy);
		//}

		RespawnEnemy();

		for (Enemy* enemy : enemies_) 
		{
			if (enemy->GetIsGrappled())
			{
				SetRespawnTimer();
			}

			enemy->Update();

			
		}

		enemies_.remove_if([this](Enemy* enemy) 
		{
			if (enemy->GetIsDead()) 
			{
				delete enemy;
				SetRespawnTimer();
				return true;
			}
			return false;
		});

		BulletManager::GetInstance()->Update();

		// カメラの処理
		if (isDebugCameraActive_) {
			debugCamera_->Update();
			camera_.matView = debugCamera_->GetCamera().matView;
			camera_.matProjection = debugCamera_->GetCamera().matProjection;
			// ビュープロジェクション行列の転送
			camera_.TransferMatrix();
		} else {
			cameraController_->Update();
			camera_.matView = cameraController_->GetCamera().matView;
			camera_.matProjection = cameraController_->GetCamera().matProjection;
			camera_.TransferMatrix();
		}

		// 衝突判定
		CheckAllCollisions();

		if (player_->GetIsDead() || Input::GetInstance()->PushKey(DIK_F2)) {
			// 死亡演出フェーズに切り換え
			phase_ = Phase::kDeath;
			// 自キャラの座標を取得
			const Vector3& deathParticlesPosition = player_->GetWorldPosition();

			// 死亡パーティクルを生成、初期化
			deathParticles_ = new DeathParticles;
			deathParticles_->Initialize(modelDeathParticle_, &camera_, deathParticlesPosition);
		}



		if (StageTime::GetInstance()->TimeUp()) {
			phase_ = Phase::kFadeOut;
			fade_->Start(Fade::Status::FadeOut, 1.0f);
		}

		break;

	case Phase::kDeath:

		// 天球の更新
		skydome_->Update();

		for (Enemy* enemy : enemies_) {
			enemy->Update();
		}

		enemies_.remove_if([](Enemy* enemy) {
			if (enemy->GetIsDead()) {
				delete enemy;
				StageTime::GetInstance()->AddScore();
				return true;
			}
			return false;
		});

		BulletManager::GetInstance()->Update();

		//// 背景の更新
		//backGround_->Update();

		// 死亡パーティクルの更新
		deathParticles_->Update();

		// カメラの処理
		if (isDebugCameraActive_) {
			debugCamera_->Update();
			camera_.matView = debugCamera_->GetCamera().matView;
			camera_.matProjection = debugCamera_->GetCamera().matProjection;
			// ビュープロジェクション行列の転送
			camera_.TransferMatrix();
		} else {
			cameraController_->Update();
			camera_.matView = cameraController_->GetCamera().matView;
			camera_.matProjection = cameraController_->GetCamera().matProjection;
			camera_.TransferMatrix();
		}

		if (deathParticles_ && deathParticles_->GetIsFinished()) 
		{
			phase_ = Phase::kFadeOut;
			fade_->Start(Fade::Status::FadeOut, 1.0f);
		}

		break;

	case Phase::kFadeOut:

		// 天球の更新
		skydome_->Update();

		for (Enemy* enemy : enemies_) {
			enemy->Update();
		}

		enemies_.remove_if([](Enemy* enemy) {
			if (enemy->GetIsDead()) {
				delete enemy;
				StageTime::GetInstance()->AddScore();
				return true;
			}
			return false;
		});

		BulletManager::GetInstance()->Update();

		//// 背景の更新
		//backGround_->Update();

		// カメラの処理
		if (isDebugCameraActive_) {
			debugCamera_->Update();
			camera_.matView = debugCamera_->GetCamera().matView;
			camera_.matProjection = debugCamera_->GetCamera().matProjection;
			// ビュープロジェクション行列の転送
			camera_.TransferMatrix();
		} else {
			cameraController_->Update();
			camera_.matView = cameraController_->GetCamera().matView;
			camera_.matProjection = cameraController_->GetCamera().matProjection;
			camera_.TransferMatrix();
		}

		if (player_)
		{
			// 自キャラの更新
			player_->Update();
		}

	    fade_->Update();

		if (fade_->IsFinished())
		{
			finished_ = true;
		}

		break;

	}

	StageTime::GetInstance()->Update();

#ifdef _DEBUG

	ImGui::Begin("game scene window");

	ImGui::DragFloat3("camera translation", &camera_.translation_.x, 0.0f);

	ImGui::End();

#endif
	

#ifdef _DEBUG
	if (Input::GetInstance()->TriggerKey(DIK_F1)) {
		isDebugCameraActive_ = !isDebugCameraActive_;
	}
#endif

	
}

void GameScene::Draw()
{
	// DirectXCommonインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 3Dモデル描画前処理
	Model::PreDraw(dxCommon->GetCommandList());


	switch (phase_) 
	{

	case Phase::kFadeIn:
		
		// 天球の描画
		skydome_->Draw();

		//// 背景の描画
		//backGround_->Draw();

		// 自キャラの描画
		player_->Draw();

		// 敵キャラの描画
		for (Enemy* enemy : enemies_)
		{
			enemy->Draw();
		}

		BulletManager::GetInstance()->Draw();

		Sprite::PreDraw(dxCommon->GetCommandList());

		SpriteDraw::GetInstance()->Draw(HowToOperate1Sprite_, 
			SRT2D{{1.0f, 1.0f}, 0.0f, {50.0f, 700.f}}, {0.0f, 1.0f}, {202.0f, 123.0f});


		if (player_->GetAnchor()->GetMode() == Anchor::Mode::kHold)
		{
			SpriteDraw::GetInstance()->Draw(HowToOperate2Sprite_, 
				SRT2D{{1.0f, 1.0f}, 0.0f, {1180.0f, 700.f}}, {1.0f, 1.0f}, {280.0f, 58.0f});
		}

		SpriteDraw::GetInstance()->Draw(HowToOperate3Sprite_, 
				SRT2D{{1.0f, 1.0f}, 0.0f, {600.0f, 700.f}}, {0.5f, 1.0f}, {306.0f, 65.0f});
		
		for (size_t i = 0; i < player_->GetHitPoint(); ++i)
		{
			SpriteDraw::GetInstance()->Draw(hitPointSprites_[i], 
			SRT2D{{1.0f, 1.0f}, 0.0f, {50.0f + i * 32.0f, 50.f}}, {0.0f, 0.0f}, {29.0f, 29.0f});
		}

		StageTime::GetInstance()->Draw();

		Sprite::PostDraw();

		fade_->Draw();

		break;

	case Phase::kPlay:

		// 天球の描画
		skydome_->Draw();

		//// 背景の描画
		//backGround_->Draw();

		// 自キャラの描画
		player_->Draw();

		// 敵キャラの描画
		for (Enemy* enemy : enemies_) {
			enemy->Draw();
		}

		BulletManager::GetInstance()->Draw();

		Sprite::PreDraw(dxCommon->GetCommandList());

		/*EX1Sprite_->Draw();
		EX2Sprite_->Draw();*/

		SpriteDraw::GetInstance()->Draw(HowToOperate1Sprite_, 
			SRT2D{{1.0f, 1.0f}, 0.0f, {50.0f, 700.f}}, {0.0f, 1.0f}, {202.0f, 123.0f});

		if (player_->GetAnchor()->GetMode() == Anchor::Mode::kHold)
		{
			SpriteDraw::GetInstance()->Draw(HowToOperate2Sprite_, 
				SRT2D{{1.0f, 1.0f}, 0.0f, {1180.0f, 700.f}}, {1.0f, 1.0f}, {280.0f, 58.0f});
		}

		SpriteDraw::GetInstance()->Draw(HowToOperate3Sprite_, 
			SRT2D{{1.0f, 1.0f}, 0.0f, {600.0f, 700.f}}, {0.5f, 1.0f}, {306.0f, 65.0f});

		for (size_t i = 0; i < player_->GetHitPoint(); ++i)
		{
			SpriteDraw::GetInstance()->Draw(hitPointSprites_[i], 
			SRT2D{{1.0f, 1.0f}, 0.0f, {50.0f + i * 32.0f, 50.f}}, {0.0f, 0.0f}, {29.0f, 29.0f});
		}

		StageTime::GetInstance()->Draw();

		Sprite::PostDraw();

		break;

	case Phase::kDeath:

		// 天球の描画
		skydome_->Draw();

		//// 背景の描画
		//backGround_->Draw();

		if (deathParticles_)
		{
			// 死亡エフェクトの描画
			deathParticles_->Draw();
		}

		// 敵キャラの描画
		for (Enemy* enemy : enemies_) {
			enemy->Draw();
		}
		
		BulletManager::GetInstance()->Draw();

		Sprite::PreDraw(dxCommon->GetCommandList());

		/*EX1Sprite_->Draw();
		EX2Sprite_->Draw();*/
		SpriteDraw::GetInstance()->Draw(HowToOperate1Sprite_, 
			SRT2D{{1.0f, 1.0f}, 0.0f, {50.0f, 700.f}}, {0.0f, 1.0f}, {202.0f, 123.0f});

		if (player_->GetAnchor()->GetMode() == Anchor::Mode::kHold)
		{
			SpriteDraw::GetInstance()->Draw(HowToOperate2Sprite_, 
				SRT2D{{1.0f, 1.0f}, 0.0f, {1180.0f, 700.f}}, {1.0f, 1.0f}, {280.0f, 58.0f});
		}

		SpriteDraw::GetInstance()->Draw(HowToOperate3Sprite_, 
			SRT2D{{1.0f, 1.0f}, 0.0f, {600.0f, 700.f}}, {0.5f, 1.0f}, {306.0f, 65.0f});

		for (size_t i = 0; i < player_->GetHitPoint(); ++i)
		{
			SpriteDraw::GetInstance()->Draw(hitPointSprites_[i], 
			SRT2D{{1.0f, 1.0f}, 0.0f, {50.0f + i * 32.0f, 50.f}}, {0.0f, 0.0f}, {29.0f, 29.0f});
		}

		StageTime::GetInstance()->Draw();

		Sprite::PostDraw();

		break;

	case Phase::kFadeOut:

		// 天球の描画
		skydome_->Draw();

		//// 背景の描画
		//backGround_->Draw();

		if (deathParticles_)
		{
			// 死亡エフェクトの描画
			deathParticles_->Draw();
		}

		if (player_) 
		{
			//// 自キャラの描画
			//player_->Draw();
		}

		// 敵キャラの描画
		for (Enemy* enemy : enemies_) {
			enemy->Draw();
		}

		BulletManager::GetInstance()->Draw();

		Sprite::PreDraw(dxCommon->GetCommandList());

		SpriteDraw::GetInstance()->Draw(HowToOperate1Sprite_, 
			SRT2D{{1.0f, 1.0f}, 0.0f, {50.0f, 700.f}}, {0.0f, 1.0f}, {202.0f, 123.0f});

		if (player_->GetAnchor()->GetMode() == Anchor::Mode::kHold)
		{
			SpriteDraw::GetInstance()->Draw(HowToOperate2Sprite_, 
				SRT2D{{1.0f, 1.0f}, 0.0f, {1180.0f, 700.f}}, {1.0f, 1.0f}, {280.0f, 58.0f});
		}

		SpriteDraw::GetInstance()->Draw(HowToOperate3Sprite_, 
			SRT2D{{1.0f, 1.0f}, 0.0f, {600.0f, 700.f}}, {0.5f, 1.0f}, {306.0f, 65.0f});

		for (size_t i = 0; i < player_->GetHitPoint(); ++i)
		{
			SpriteDraw::GetInstance()->Draw(hitPointSprites_[i], 
			SRT2D{{1.0f, 1.0f}, 0.0f, {50.0f + i * 32.0f, 50.f}}, {0.0f, 0.0f}, {29.0f, 29.0f});
		}

		StageTime::GetInstance()->Draw();

		/*EX1Sprite_->Draw();
		EX2Sprite_->Draw();*/

		Sprite::PostDraw();

		fade_->Draw();

		break;

	}

#ifdef _DEBUG
	AxisIndicator::GetInstance()->Draw();
#endif
	// スプライトの描画処理

	
	// 3Dモデル描画後処理
	Model::PostDraw();

}

void GameScene::SetRespawnTimer() 
{
	float respawnTimer = SeedManager::GetInstance()->GenerateFloat(3.0f, 5.0f); 
	respawnTimers_.push_back(respawnTimer);
}

void GameScene::RespawnEnemy() {

	 for (auto it = respawnTimers_.begin(); it != respawnTimers_.end();) 
	 {
		*it -= kDeltaTime;
		if (*it <= 0.0f) 
		{
			it = respawnTimers_.erase(it);

			int respawnNum;
			int randValue = SeedManager::GetInstance()->GenerateInt(0, 100);

			if (randValue >= 90.0f) 
			{
				respawnNum = 3;
			} 
			else if (randValue >= 50.0f)
			{
				respawnNum = 2;
			} 
			else
			{
				respawnNum = 1;
			}

			if (enemies_.size() + respawnNum > maxEnemies_) 
			{
				respawnNum = maxEnemies_ - static_cast<int>(enemies_.size());
			}


			for (int i = 0; i < respawnNum; i++)
			{
				Enemy* enemy = new Enemy();
				enemy->Initialize(
				    modelEnemy_, &camera_,
				    Vector3
					{
						SeedManager::GetInstance()->GenerateInt(-5, 5) * 0.5f, 
						SeedManager::GetInstance()->GenerateInt(-5, 5) * 0.5f, 
						SeedManager::GetInstance()->GenerateInt(30, 35) * 1.0f
					}
				);
				enemy->SetPlayer(player_);
				enemies_.push_back(enemy);
			}

			return;
			
		} 
		else 
		{
			++it;
		}
	}
}

void GameScene::CheckAllCollisions() {
	#pragma region 自キャラと敵Mobの当たり判定
	{
		//// 判定対象1と2の距離
		//AABB aabb1, aabb2;

		//// 自キャラの座標
		//aabb1 = player_->GetAABB();

		//// 自キャラと敵Mob全ての当たり判定
		//for (Enemy* enemy : enemies_)
		//{
		//	if (enemy->IsCollisionDisabled())
		//	{
		//		// 衝突無効フラグが立っている場合はスキップ
		//		continue;
		//	}

		//	// 敵Mobの座標
		//	aabb2 = enemy->GetAABB();

		//	// AABB同士の交差判定
		//	if (IsCollision(aabb1, aabb2))
		//	{
		//		// 自キャラの衝突時間数を呼び出す
		//		player_->OnCollision(enemy);
		//		// 敵の衝突時関数を呼び出す
		//		enemy->OnCollision(player_);
		//	}
		//}
	}
	#pragma endregion

	#pragma region 自キャラとゴールの当たり判定
	{
		//// 判定対象1と2の距離
		//AABB aabb1, aabb2;

		//// 自キャラの座標
		//aabb1 = player_->GetAABB();


		//// 敵Mobの座標
		//aabb2 = goal_->GetAABB();

		//// AABB同士の交差判定
		//if (IsCollision(aabb1, aabb2)) 
		//{
		//	// 敵の衝突時関数を呼び出す
		//	goal_->OnCollision(player_);
		//}
		
	}
	#pragma endregion

	#pragma region 自弾と敵Mobの当たり判定
	{
		// 判定対象1と2の距離
		AABB aabb1, aabb2;

		// 自弾の座標
		for (Bullet* bullet : BulletManager::GetInstance()->GetBullets())
		{
			if (bullet->GetID() != Bullet::ID::kPlayer)
			{
				continue;
			}

			aabb1 = bullet->GetAABB();

			// 自キャラと敵Mob全ての当たり判定
			for (Enemy* enemy : enemies_) 
			{
				if (enemy->GetIsCollisionDisabled()) 
				{
					// 衝突無効フラグが立っている場合はスキップ
					continue;
				}

				if (enemy->GetBehavior() == Enemy::Behavior::kGrappled) {
					// 掴まれている場合はスキップ
					continue;
				}

				// 敵Mobの座標
				aabb2 = enemy->GetAABB();

				// AABB同士の交差判定
				if (IsCollision(aabb1, aabb2)) 
				{
					// 自弾の衝突時間数を呼び出す
					bullet->OnCollision(enemy);
					// 敵の衝突時関数を呼び出す
					enemy->OnCollision(bullet);
				}
			}
		}
	}
	#pragma endregion

	#pragma region アンカーと敵Mobの当たり判定
	{
		Anchor* anchor = player_->GetAnchor();

		// 判定対象1と2の距離
		AABB aabb1, aabb2;

		// 自弾の座標
		aabb1 = anchor->GetAABB();
		// 自キャラと敵Mob全ての当たり判定
		for (Enemy* enemy : enemies_) 
		{
			if (enemy->GetIsCollisionDisabled()) {
				// 衝突無効フラグが立っている場合はスキップ
				continue;
			}

			// 敵Mobの座標
			aabb2 = enemy->GetAABB();

			// AABB同士の交差判定
			if (IsCollision(aabb1, aabb2) && anchor->GetMode() == Anchor::Mode::kFoward) 
			{
				// アンカーの衝突時間数を呼び出す
				anchor->OnCollision(enemy);
				// 敵の衝突時関数を呼び出す
				enemy->OnCollision(anchor);
			}
		}
	}
	#pragma endregion

	#pragma region 敵弾と自キャラの当たり判定
	{
		// 判定対象1と2のAABB
		AABB aabb1, aabb2;
		// 自キャラの座標
		aabb1 = player_->GetAABB();
		// 敵弾全ての当たり判定
		for (Bullet* bullet : BulletManager::GetInstance()->GetBullets()) 
		{
			if (bullet->GetID() != Bullet::ID::kEnemy) 
			{
				continue;
			}

			aabb2 = bullet->GetAABB();
			// AABB同士の交差判定
			if (IsCollision(aabb1, aabb2)) 
			{
				// 自キャラの衝突時間数を呼び出す
				player_->OnCollision(bullet);
				// 敵弾の衝突時関数を呼び出す
				bullet->OnCollision(player_);
				cameraController_->OnShake();
			}
		}
	}
	#pragma endregion

	#pragma region 掴まれ状態の敵と敵弾の当たり判定
	{
		for (Enemy* enemy : enemies_) {
			if (enemy->GetBehavior() != Enemy::Behavior::kGrappled) 
			{
				// 掴まれてない場合はスキップ
				continue;
			}

			// 判定対象1と2のAABB
			AABB aabb1, aabb2;

			// 敵の座標
			aabb1 = enemy->GetAABB();

			// 敵弾全ての当たり判定
			for (Bullet* bullet : BulletManager::GetInstance()->GetBullets()) 
			{
				if (bullet->GetID() != Bullet::ID::kEnemy) 
				{
					continue;
				}

				aabb2 = bullet->GetAABB();
				// AABB同士の交差判定
				if (IsCollision(aabb1, aabb2)) {
					// 自キャラの衝突時間数を呼び出す
					enemy->OnCollision(bullet);
					// 敵弾の衝突時関数を呼び出す
					bullet->OnCollision(enemy);
				}
			}
		}
	}
	#pragma endregion

	#pragma region 射出状態の敵と敵の当たり判定
	{
		for (Enemy* enemy1 : enemies_) {
			if (enemy1->GetBehavior() != Enemy::Behavior::kShoot) {
				// 射出されていない場合はスキップ
				continue;
			}

			// 判定対象1と2のAABB
			AABB aabb1, aabb2;

			// 敵の座標
			aabb1 = enemy1->GetAABB();

			// 対象以外の敵全ての当たり判定
			for (Enemy* enemy2 : enemies_) 
			{
				if (enemy1 == enemy2) 
				{
					// 自分自身はスキップ
					continue;
				}

				if (enemy2->GetBehavior() == Enemy::Behavior::kShoot)
				{
					// 同じ射出状態の敵はスキップ
					continue;
				}

				aabb2 = enemy2->GetAABB();
				// AABB同士の交差判定
				if (IsCollision(aabb1, aabb2)) {
					// 自キャラの衝突時間数を呼び出す
					enemy1->OnCollision(enemy2);
					// 敵弾の衝突時関数を呼び出す
					enemy2->OnCollision(enemy1);
				}
			}
		}
	}
	#pragma endregion
}

void GameScene::ChangePhase()
{ 
	switch (phase_) 
	{ 
		case Phase::kPlay:


			break;

		case Phase::kDeath:

			

			break;
	}
}

