#pragma once
#include "Framework.h"

// ゲーム全体
class Game : public Dict_Framework
{
public:

	// 初期化
	void Initialize() override;

	// 終了
	void Finalize() override;

	// 更新
	void Update() override;

	// 描画
	void Draw() override;

private:

	std::vector<std::unique_ptr<Sprite>> sprites_;

	std::vector<std::unique_ptr<Object3d>> object3ds_;

	std::list<std::unique_ptr<ParticleEmitter>> emitters_;

private:

	bool isDrawSprite_ = false;

	bool isDrawObject3d_ = true;


	DebugCamera debugCamera_;
	bool isDebugCamera_ = false;

	Vector2 mousePosition_ = { 0.0f, 0.0f };
};

