#include "Anchor.h"
#include "assert.h"
#include "WorldTransformAssist.h"
#include "Player.h"

void Anchor::Initialize(
	Model* model, const Camera* camera, const Player* player, 
	const Vector3& position, const Vector3& velocity) 
{
	assert(model);

	model_ = model;
	// テクスチャ読み込み
	textureHandle_ = TextureManager::Load("uvChecker.png");

	camera_ = camera;

	player_ = player;

	worldTransform_.Initialize();
	worldTransform_.scale_ = {1.5f, 3.0f, 1.5f};
	worldTransform_.rotation_ = {0.0f, 0.0f, 0.0f};
	worldTransform_.translation_ = position;

	mode_ = Anchor::Mode::kWait;

	velocity_ = velocity;

	shotSEDataHandle_ = Audio::GetInstance()->LoadWave("SE/shotAnchor.wav");
	hitSEDataHandle_ = Audio::GetInstance()->LoadWave("SE/hitAnchor.wav");
	shootSEDataHandle_ = Audio::GetInstance()->LoadWave("SE/shootAnchor.wav");

}

void Anchor::Update() 
{
	isFinished_ = false;

	switch (mode_)
	{ 
	case Anchor::Mode::kWait:
		
		worldTransform_.rotation_ = player_->GetRotation();
		worldTransform_.translation_ = player_->GetTranslation();

		isShoot_ = false;

		break;

	case Anchor::Mode::kFoward:

		worldTransform_.translation_ += velocity_;

		length_ = Length(worldTransform_.translation_ - player_->GetTranslation());

		if (length_ >= kMaxLength)
		{
			mode_ = Anchor::Mode::kBack;
			isPlayShotSE_ = false;
		}

		break;

	case Anchor::Mode::kBack:

		// playerの位置に戻す
		toTarget = player_->GetTranslation() - worldTransform_.translation_;
		direction = Normalize(toTarget);
		desiredVel = direction * maxSpeed;

		velocity_ = desiredVel;
		worldTransform_.translation_ += velocity_;
		length_ -= Length(velocity_);

		if (length_ <= 0.0f) 
		{
			mode_ = Anchor::Mode::kWait;
			isFinished_ = true;
		}

		isPlayShotSE_ = false;

		break;

	case Anchor::Mode::kGrapple:

		// playerの位置に戻す
		toTarget = player_->GetTranslation() - worldTransform_.translation_;
		direction = Normalize(toTarget);
		desiredVel = direction * maxSpeed;

		velocity_ = desiredVel;
		worldTransform_.translation_ += velocity_;
		length_ -= Length(velocity_);

		if (length_ <= 1.0f) {
			mode_ = Anchor::Mode::kHold;
			isFinished_ = true;
		}

		isPlayShotSE_ = false;

		break;

	case Anchor::Mode::kHold:

		isPlayHitSE_ = false;

		worldTransform_.rotation_ = player_->GetRotation();
		worldTransform_.translation_ = player_->GetTranslation() + TransformNormal({0.0f, 0.0f, 1.0f}, worldTransform_.matWorld_);

		if (isShoot_)
		{
			mode_ = Anchor::Mode::kWait;
			isFinished_ = true;
		}

		break;

	}

	

	WorldTransformUpdate(worldTransform_);

#ifdef _DEBUG

	ImGui::Begin("anchor window");

	ImGui::DragFloat3("rotation", &worldTransform_.rotation_.x, 0.0f);

	ImGui::DragFloat3("translation", &worldTransform_.translation_.x, 0.0f);

	ImGui::Text("length : %f", Length(worldTransform_.translation_ - player_->GetTranslation()));

	ImGui::Text("Mode : %d", static_cast<int>(mode_));

	ImGui::Text("isFinished : %d", static_cast<int>(isFinished_));

#endif
}

void Anchor::Draw() 
{ 
	switch (mode_) {
	case Anchor::Mode::kWait:
		
		break;

	case Anchor::Mode::kFoward:

		model_->Draw(worldTransform_, *camera_); 

		break;

	case Anchor::Mode::kBack:

		model_->Draw(worldTransform_, *camera_); 

		break;

	case Anchor::Mode::kGrapple:

		model_->Draw(worldTransform_, *camera_); 

		break;

	case Anchor::Mode::kHold:

		model_->Draw(worldTransform_, *camera_);

		break;
	}

	
}

void Anchor::OnCollision(const Enemy* enemy) 
{ 
	(void)enemy; 
	mode_ = Anchor::Mode::kGrapple;
	PlaySEHit();
}

void Anchor::Shot() 
{ 
	isShotEnemyBullet_ = true;
}


void Anchor::Shoot(const Vector3& velocity) 
{ 
	if (mode_ == Anchor::Mode::kHold)
	{
		isShoot_ = true;
		PlaySEShoot();
		isPlayShootSE_ = false;
		return;
	}

	mode_ = Anchor::Mode::kFoward; 
	PlaySEShot();

	velocity_ = velocity;
}

const Vector3 Anchor::GetWorldPosition() const 
{
	// ワールド座標を入れる変数
	Vector3 worldPos;

	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

AABB Anchor::GetAABB() 
{
	Vector3 worldPos = GetWorldPosition();

	AABB aabb;

	aabb.min = {worldPos.x - size_.x, worldPos.y - size_.y, worldPos.z - size_.z};
	aabb.max = {worldPos.x + size_.x, worldPos.y + size_.y, worldPos.z + size_.z};

	return aabb;
}

void Anchor::PlaySEShot() { 
	if (!isPlayShotSE_)
	{
		shotSEHandle_ = Audio::GetInstance()->PlayWave(shotSEDataHandle_, false, 0.4f);
		isPlayShotSE_ = true;
	}
	
}

void Anchor::PlaySEHit() 
{
	if (!isPlayHitSE_) 
	{
		hitSEHandle_ = Audio::GetInstance()->PlayWave(hitSEDataHandle_, false, 0.2f);
		isPlayHitSE_ = true;
	}
}

void Anchor::PlaySEShoot() 
{
	if (!isPlayShootSE_) 
	{
		shootSEHandle_ = Audio::GetInstance()->PlayWave(shootSEDataHandle_, false, 0.2f);
		isPlayShootSE_ = true;
	}
}
