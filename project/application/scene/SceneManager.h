#pragma once
#include <memory>
#include "BaseScene.h"
#include "AbstractSceneFactory.h"

class SceneManager
{

public:

	void Initialize(const std::string& sceneName);

	void Update();

	void Draw();

	void ChangeScene(const std::string& sceneName);

public:	// 外部入出力

	const std::string GetNextScene() const;

	void SetSceneRequest(const std::string& sceneRequest){ sceneRequest_ = sceneRequest; }

public:

	// シングルトンインスタンスの取得
	static SceneManager* GetInstance();
	// 終了
	void Finalize();

	// コンストラクタに渡すための鍵
	class ConstructorKey
	{
	private:
		ConstructorKey() = default;
		friend class SceneManager;
	};

	// PassKeyを受け取るコンストラクタ
	explicit SceneManager(ConstructorKey){}

private:

	// unique_ptr の型定義に Deleter を入れることでdeleteが可能になる
	static std::unique_ptr<SceneManager> instance_;

	~SceneManager() = default;
	SceneManager(SceneManager&) = delete;
	SceneManager& operator=(SceneManager&) = delete;

	friend struct std::default_delete<SceneManager>;


private:

	std::string scene_;

	std::string sceneRequest_;

	// シーンファクトリー
	std::unique_ptr<AbstractSceneFactory> sceneFactory_;

	std::unique_ptr<BaseScene> currentScene_;

};

