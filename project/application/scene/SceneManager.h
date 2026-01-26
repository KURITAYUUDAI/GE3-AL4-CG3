#pragma once
#include <memory>
#include "BaseScene.h"
#include "AbstractSceneFactory.h"

class SceneManager
{

public:
	// シングルトンインスタンスの取得
	static SceneManager* GetInstance();
	// 終了
	void Finalize();

public:

	void Initialize(const std::string& sceneName);

	void Update();

	void Draw();

	void ChangeScene(const std::string& sceneName);

public:	// 外部入出力

	const std::string GetNextScene() const;

	void SetSceneRequest(const std::string& sceneRequest){ sceneRequest_ = sceneRequest; }

	void SetSceneFactory(std::shared_ptr<AbstractSceneFactory> sceneFactory){ sceneFactory_ = sceneFactory; }

private:	// シングルトンインスタンス

	struct Deleter
	{
		void operator()(SceneManager* p) const
		{
			// クラス内部のスコープなのでprivateなデストラクタを呼べる
			delete p;
		}
	};

	static std::unique_ptr<SceneManager, Deleter> instance_;

	SceneManager() = default;
	~SceneManager() = default;
	SceneManager(SceneManager&) = delete;
	SceneManager& operator=(SceneManager&) = delete;

private:

	std::string scene_;

	std::string sceneRequest_;

	// シーンファクトリー
	std::shared_ptr<AbstractSceneFactory> sceneFactory_;

	std::unique_ptr<BaseScene> currentScene_;

};

