#include "Game.h"
#include "SceneFactory.h"
#include "PostEffectManager.h"
#include "GaussianBlur.h"
#include "Outline.h"

void Game::Initialize()
{
	Dict_Framework::Initialize();

	// 使えるエフェクトの種類を登録
	postEffectManager_->RegisterFactory("GaussianBlur",
		[]{ return std::make_unique<GaussianBlur>(); });
	postEffectManager_->RegisterFactory("Outline",
		[]{ return std::make_unique<Outline>(); });


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

	postEffectManager_->Finalize();

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
	DirectXBase::GetInstance()->PreDraw();

	srvManager_->PreDraw();
	
	sceneManager_->Draw();

	DirectXBase::GetInstance()->DrawSwapChain();

	postEffectManager_->Draw(
		offscreenRender_->GetRenderTextureResource(),
		offscreenRender_->GetSRVIndex());

	imguiManager_->Draw();

	dxBase_->PostDraw();
}

