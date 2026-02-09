#include "ResultScene.h"
#include "SpriteDraw.h"
#include "StageTime.h"

ResultScene::~ResultScene() 
{
	delete fade_;
	/*delete TEX1Sprite_;*/
	delete TEX2Sprite_;
	delete modelPlayer_;

	StageTime::GetInstance()->Finalize();
}

void ResultScene::Initialize() {
	phase_ = Phase::kFadeIn;

	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);

	// 3Dモデルの生成
	modelPlayer_ = Model::CreateFromOBJ("player", true);
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);

	worldTransformPlayer_.Initialize();
	worldTransformPlayer_.translation_ = {0.0f, 0.0f, 0.0f};
	worldTransformPlayer_.scale_ = {10.0f, 10.0f, 10.0f};
	worldTransformPlayer_.rotation_ = {0.0f, 0.0f, 0.0f};

	// 天球の生成
	skydome_ = new Skydome();
	// 天球の初期化
	skydome_->Initialize(modelSkydome_, &camera_);

	// カメラの初期化
	camera_.farZ = 1000.0f;
	camera_.Initialize();

	decisionSEDataHandle_ = Audio::GetInstance()->LoadWave("SE/decision.wav");
	isPlayDecisionSE_ = false;
}

void ResultScene::Update() {
	worldTransformPlayer_.matWorld_ = MakeAffineMatrixB(worldTransformPlayer_.scale_, worldTransformPlayer_.rotation_, worldTransformPlayer_.translation_);
	worldTransformPlayer_.TransferMatrix();

	switch (phase_) {
	case Phase::kFadeIn:

		// 天球の更新
		skydome_->Update();

		fade_->Update();

		if (fade_->IsFinished()) {
			phase_ = Phase::kMain;
		}

		break;

	case Phase::kMain:

		// 天球の更新
		skydome_->Update();

		if (Input::GetInstance()->PushKey(DIK_SPACE)) {
			phase_ = Phase::kFadeOut;
			fade_->Start(Fade::Status::FadeOut, 1.0f);
			PlaySEDecision();
		}

		break;

	case Phase::kFadeOut:

		// 天球の更新
		skydome_->Update();

		fade_->Update();

		if (fade_->IsFinished()) {
			finished_ = true;
		}

		break;
	}

	// 行列を更新

	camera_.UpdateMatrix();

	// 行列を転送

	camera_.TransferMatrix();
}

void ResultScene::Draw() 
{
	// DirectXCommonインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 3Dモデル描画前処理
	Model::PreDraw(dxCommon->GetCommandList());

	switch (phase_) {

	case Phase::kFadeIn:

		skydome_->Draw();

		Sprite::PreDraw(dxCommon->GetCommandList());

		/*TEX1Sprite_->Draw();*/
		TEX2Sprite_->Draw();

		SpriteDraw::GetInstance()->Draw(gameOverSprite_,
			SRT2D{{1.0f, 1.0f}, 0.0f, {640.0f, 250.f}}, {0.5f, 0.5f}, {645.0f, 111.0f});

		StageTime::GetInstance()->ResultDraw();

		Sprite::PostDraw();

		fade_->Draw();

		break;

	case Phase::kMain:

		skydome_->Draw();

		Sprite::PreDraw(dxCommon->GetCommandList());

		/*TEX1Sprite_->Draw();*/
		TEX2Sprite_->Draw();

		SpriteDraw::GetInstance()->Draw(gameOverSprite_,
			SRT2D{{1.0f, 1.0f}, 0.0f, {640.0f, 250.f}}, {0.5f, 0.5f}, {645.0f, 111.0f});

		StageTime::GetInstance()->ResultDraw();

		Sprite::PostDraw();

		break;

	case Phase::kFadeOut:

		skydome_->Draw();

		Sprite::PreDraw(dxCommon->GetCommandList());

		SpriteDraw::GetInstance()->Draw(gameOverSprite_,
			SRT2D{{1.0f, 1.0f}, 0.0f, {640.0f, 250.f}}, {0.5f, 0.5f}, {645.0f, 111.0f});

		StageTime::GetInstance()->ResultDraw();

		/*TEX1Sprite_->Draw();*/
		TEX2Sprite_->Draw();

		Sprite::PostDraw();

		fade_->Draw();

		break;
	}
}

void ResultScene::PlaySEDecision() 
{
	if (!isPlayDecisionSE_) {
		decisionSEHandle_ = Audio::GetInstance()->PlayWave(decisionSEDataHandle_, false, 0.4f);
		isPlayDecisionSE_ = true;
	}
}
