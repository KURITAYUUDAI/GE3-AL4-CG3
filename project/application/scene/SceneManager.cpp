#include "SceneManager.h"
#include "GamePlayScene.h"
#include "effect/FadeManager.h"

std::unique_ptr<SceneManager> SceneManager::instance_ = nullptr;

SceneManager* SceneManager::GetInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = std::make_unique<SceneManager>(ConstructorKey());
	}
	return instance_.get();
}

void SceneManager::Finalize()
{
	if (currentScene_)
	{
		currentScene_->Finalize();
		currentScene_.reset();
	}

	instance_.reset();
}

void SceneManager::Initialize(const std::string& sceneName)
{
	sceneFactory_ = std::make_unique<SceneFactory>();

	scene_ = sceneName;
	sceneRequest_ = sceneName;
	ChangeScene(sceneName);
}

void SceneManager::Update(const float& deltaTime)
{
	using enum FadeManager::Status;
	using enum FadeManager::FadeType;
	switch (fadeStatus_)
	{
	case None:

		break;

	case FadeIn:

		if (FadeManager::GetInstance()->IsFinished(Dissolve))
		{
			fadeStatus_ = None;
		}

		break;

	case FadeOut:

		if (FadeManager::GetInstance()->IsFinished(Dissolve))
		{
			// シーン変更
			ChangeScene(sceneRequest_);
		}

		break;
	}

	//if (sceneRequest_ != scene_)
	//{
	//	// シーン変更
	//	ChangeScene(sceneRequest_);
	//}

	FadeManager::GetInstance()->Update();

	if (currentScene_)
	{
		currentScene_->Update(deltaTime);
	}
}

void SceneManager::Draw()
{
	if (currentScene_)
	{
		currentScene_->Draw();
	}
}

void SceneManager::ChangeScene(const std::string& sceneName)
{
	// 現在のシーンを終了し内部をリセットする
	if (currentScene_)
	{
		currentScene_->Finalize();
		currentScene_.reset();
	}

	assert(sceneFactory_ && "sceneFactory_ is nullptr");

	currentScene_ = sceneFactory_->CreateScene(sceneName);

	// currentSceneがnullptrで無い場合は初期化を行う
	if (!currentScene_)
	{
		assert(false && "CreateScene returned nullptr");
		return;
	}

	scene_ = sceneName;
	currentScene_->Initialize();

	fadeStatus_ = FadeManager::Status::FadeIn;
	FadeManager::GetInstance()->Start(FadeManager::FadeType::Dissolve,
		fadeStatus_, 0.5f);
}

const std::string SceneManager::GetNextScene() const
{
	if (scene_ == "TITLE")
	{
		return "TITLE";
	}
	else if (scene_ == "GAMEPLAY")
	{
		return "GAMEPLAY";
	}
	else
	{
		return "UNKNOWN";
	}
}

void SceneManager::SetSceneRequest(const std::string& sceneRequest)
{
	if (fadeStatus_ != FadeManager::Status::None) return;

	sceneRequest_ = sceneRequest;

	if (sceneRequest_ != scene_)
	{
		fadeStatus_ = FadeManager::Status::FadeOut;
		FadeManager::GetInstance()->Start(FadeManager::FadeType::Dissolve, 
			FadeManager::Status::FadeOut, 0.5f);
	}
}
