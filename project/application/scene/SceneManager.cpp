#include "SceneManager.h"
#include "GamePlayScene.h"

std::unique_ptr<SceneManager, SceneManager::Deleter> SceneManager::instance_ = nullptr;

SceneManager* SceneManager::GetInstance()
{
	if (instance_ == nullptr)
	{
		instance_.reset(new SceneManager);
	}
	return instance_.get();
}


void SceneManager::Initialize(const std::string& sceneName)
{
	scene_ = sceneName;
	sceneRequest_ = sceneName;
	ChangeScene(sceneName);
}

void SceneManager::Update()
{
	if (sceneRequest_ != scene_)
	{
		// シーン変更
		ChangeScene(sceneRequest_);
	}

	if (currentScene_)
	{
		currentScene_->Update();
	}
}

void SceneManager::Draw()
{
	if (currentScene_)
	{
		currentScene_->Draw();
	}
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
