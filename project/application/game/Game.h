#pragma once
#include "Framework.h"
#include "BaseScene.h"
#include "AbstractSceneFactory.h"
#include "SceneManager.h"

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


	// シーンファクトリー
	AbstractSceneFactory* sceneFactory_ = nullptr;

};

