#pragma once
#include "BaseScene.h"
#include "Framework.h"

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

	// 3Dオブジェクトマネージャー
	Object3dManager* object3dManager_ = Object3dManager::GetInstance();

private:

	bool isDrawSprite_ = false;

	bool isDrawObject3d_ = true;

	// カメラ
	std::unique_ptr<Camera> camera_ = nullptr;

	DebugCamera debugCamera_;
	bool isDebugCamera_ = false;

	Vector2 mousePosition_ = { 0.0f, 0.0f };
};

