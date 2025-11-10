#pragma once
#include <Windows.h>
#include <memory>
#include <map>
#include <string>

class DirectXBase;

class ModelBase;
class Model;

// モデルマネージャー
class ModelManager
{

public:

	// シングルトンインスタンスの取得
	static ModelManager* GetInstance();
	// 初期化
	void Initialize(DirectXBase* dxBase);
	// 終了
	void Finalize();

	/// <summary>
	/// モデルファイルの読み込み
	/// </summary>
	/// <param name="filePath">モデルファイルのパス</param>
	void LoadModel(const std::string& filePath);

	/// <summary>
	/// モデルの検索
	/// </summary>
	/// <param name="filePath">モデルファイルのパス</param>
	/// <returns>モデル</returns>
	Model* FindModel(const std::string& filePath);

public: // 外部入出力

	ModelBase* GetModelBase(){ return modelBase_; }

private:	// シングルトン化

	static ModelManager* instance;
	
	ModelManager() = default;
	~ModelManager() = default;
	ModelManager(ModelManager&) = delete;
	ModelManager& operator=(ModelManager&) = delete;

private:	// 静的関数

	

private:

	ModelBase* modelBase_ = nullptr;

	// モデルデータ
	std::map<std::string, std::unique_ptr<Model>> models_;

};

