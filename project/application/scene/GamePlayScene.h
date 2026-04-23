#pragma once
#include "BaseScene.h"
#include "Framework.h"
#include "SkyBox.h"
#include "Terrain.h"

#include "LightManager.h"
#include "CameraManager.h"

class GamePlayScene : public BaseScene
{
public:

	~GamePlayScene() override = default;

public:

	void Initialize() override;

	void Finalize() override;

	void Update() override;

	void Draw() override;

private:

	std::vector<std::unique_ptr<Sprite>> sprites_;

	std::vector<std::unique_ptr<Object3d>> object3ds_;

	std::list<std::unique_ptr<ParticleEmitter>> emitters_;

private:

	// テクスチャマネージャー
	TextureManager* textureManager_ = TextureManager::GetInstance();

	// パーティクルマネージャー
	ParticleManager* particleManager_ = ParticleManager::GetInstance();

	// インプットマネージャー
	InputManager* inputManager_ = InputManager::GetInstance();

	// スプライトマネージャー
	SpriteManager* spriteManager_ = SpriteManager::GetInstance();

	// モデルマネージャー
	ModelManager* modelManager_ = ModelManager::GetInstance();

	// 3Dオブジェクトマネージャー
	Object3dManager* object3dManager_ = Object3dManager::GetInstance();

	// サウンドマネージャー
	SoundManager* soundManager_ = SoundManager::GetInstance();

	// ライトマネージャー
	LightManager* lightManager_ = LightManager::GetInstance();

	// カメラマネージャー
	CameraManager* cameraManager_ = CameraManager::GetInstance();

private:

	bool isDrawSprite_ = false;

	bool isDrawObject3d_ = true;

	// カメラ
	std::unique_ptr<Camera> camera_ = nullptr;

	std::unique_ptr<DebugCamera> debugCamera_;
	bool isDebugCamera_ = false;

	Vector2 mousePosition_ = { 0.0f, 0.0f };

	SkyBox* skyBox_ = nullptr;

	std::unique_ptr<Terrain> terrain_ = nullptr;
};

