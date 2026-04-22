#pragma once
#include "myMath.h"
#include "DirectXBase.h"
#include <unordered_map>

class Camera;

class CameraManager
{
public:
	// シングルトンインスタンスの取得
	static CameraManager* GetInstance();
	// 終了
	void Finalize();

	// コンストラクタに渡すための鍵
	class ConstructorKey
	{
	private:
		ConstructorKey() = default;
		friend class CameraManager;
	};

	// PassKeyを受け取るコンストラクタ
	explicit CameraManager(ConstructorKey){}

private: 	// シングルトンインスタンス

	static std::unique_ptr<CameraManager> instance_;

	~CameraManager() = default;
	CameraManager(CameraManager&) = delete;
	CameraManager& operator=(CameraManager&) = delete;

	friend struct std::default_delete<CameraManager>;

public:

	struct CameraForGPU
	{
		Vector3 worldPosition;
	};

public:

	// 初期化
	void Initialize();
	void Update();

	void SetCbufferCameraResource(UINT RootParameterIndex);

	void AddCamera(const std::string& name, Camera* camera);

public: // 外部入出力

	// カメラリソースのGPUアドレス
	D3D12_GPU_VIRTUAL_ADDRESS GetCameraResourceGPUAddress() const { return cameraResource_->GetGPUVirtualAddress(); }
	// 現在アクティブなカメラを取得
	Camera* GetActiveCamera() const { return activeCamera_; }
	
	const Vector3& GetCameraWorldPosition() const { return cameraData_->worldPosition; }
	
	// カメラをアクティブにする
	void SetActiveCamera(const std::string& name);
	void SetCameraWorldPosition(const Vector3& worldPosition){ cameraData_->worldPosition = worldPosition; }

private:

	// カメラ用リソースを作成
	void CreateCameraResource();

private:
	std::unordered_map<std::string, Camera*> cameras_;
	Camera* activeCamera_ = nullptr;

	// カメラ用リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> cameraResource_ = nullptr;
	// バッファリソース内のデータを指すポインタ
	CameraForGPU* cameraData_ = nullptr;
};

