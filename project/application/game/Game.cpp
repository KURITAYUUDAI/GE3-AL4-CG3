#include "Game.h"
#include "SceneFactory.h"

void Game::Initialize()
{
	Dict_Framework::Initialize();


	sceneFactory_ = std::make_shared<SceneFactory>();

	sceneManager_->SetSceneFactory(sceneFactory_);

	// シーンマネージャーに最初のシーンをセット
	sceneManager_->Initialize("TITLE");

	/*sceneManager_->ChangeScene("GAMEPLAY");*/

	// クラッシュ用
	/*uint32_t* p = nullptr;
	*p = 100;*/
}

void Game::Finalize()
{
	sceneManager_->Finalize();

	Dict_Framework::Finalize();
}

void Game::Update()
{
	// 規定クラスの往診処理
	Dict_Framework::Update();

	imguiManager_->Begin();

	sceneManager_->Update();

	imguiManager_->End();
}

void Game::Draw()
{
	// 描画の処理
	dxBase_->PreDraw();

	srvManager_->PreDraw();

	
	sceneManager_->Draw();


	imguiManager_->Draw();


	dxBase_->PostDraw();
}

