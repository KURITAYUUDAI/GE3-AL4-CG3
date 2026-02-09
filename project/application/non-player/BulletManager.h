#pragma once
#include "KamataEngine.h"
#include "myMath.h"

class Bullet;

class BulletManager 
{

public:
	// シングルトンインスタンスの取得
	static BulletManager* GetInstance();
	// 終了
	void Finalize();

public:
	// 初期化
	void Initialize(Model* playerBulletModel, Model* enemyBulletModel, Camera* camera);

	// 更新
	void Update();

	// 描画
	void Draw();

	// 味方弾の生成
	void CreatePlayerBullet(const Vector3& position, const Vector3& velocity);

	// 敵弾の生成
	void CreateEnemyBullet(const Vector3& position, const Vector3& velocity);

public:  // 外部入出力

	std::list<Bullet*>& GetBullets() { return bullets_; }

	const Matrix4x4 GetBillboardWorldMatrix(const Vector3& scale, const Vector3& translate) const;

public:
	void PlaySEShot();

private: // シングルトンインスタンス
	static BulletManager* instance_;

	BulletManager() = default;
	~BulletManager() = default;
	BulletManager(BulletManager&) = delete;
	BulletManager& operator=(BulletManager&) = delete;

private:

	// モデル
	Model* modelPlayerBullet_ = nullptr;

	// モデル
	Model* modelEnemyBullet_ = nullptr;

	// カメラ
	Camera* camera_ = nullptr;

	// 弾のコンテナ
	std::list<Bullet*> bullets_;

	// ワールドに存在できる弾の最大数
	static inline const int kMaxBullet = 100;

	// ビルボード行列
	Matrix4x4 billboardMatrix_;

private:

	uint32_t shotSEDataHandle_;
	uint32_t shotSEHandle_;
	bool isPlayShotSE_;
};
