#pragma once
#include <memory>
#include <d3d12.h>
#include <wrl.h>
#include "myMath.h"
#include "StringUtility.h"

class OffscreenRender
{
public:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	// シングルトンインスタンスの取得
	static OffscreenRender* GetInstance();
	// 終了
	void Finalize();

	// コンストラクタに渡すための鍵
	class ConstructorKey
	{
	private:
		ConstructorKey() = default;
		friend class OffscreenRender;
	};

	// PassKeyを受け取るコンストラクタ
	explicit OffscreenRender(ConstructorKey){}

private: 	// シングルトンインスタンス

	static std::unique_ptr<OffscreenRender> instance_;

	~OffscreenRender() = default;
	OffscreenRender(OffscreenRender&) = delete;
	OffscreenRender& operator=(OffscreenRender&) = delete;

	friend struct std::default_delete<OffscreenRender>;


public:

	void Initialize();

	void Draw();

public:

	ID3D12Resource* GetRenderTextureResource() { return renderTextureResource_.Get(); }
	D3D12_CPU_DESCRIPTOR_HANDLE GetRTVHandle() { return rtvHandle_; }
	bool IsReady(){ return isReady_; }
	Vector4 GetClearColor() { return kRenderTargetClearValue_; }
	uint32_t GetSRVIndex() const { return renderTextureSRVIndex_; }

private:

	// RenderTextureのリソース
	ComPtr<ID3D12Resource>      renderTextureResource_;
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle_{};
	bool isReady_ = false;

	// RenderTextureのClearColor
	const Vector4 kRenderTargetClearValue_{ 0.1f, 0.25f, 0.5f, 1.0f };
	// RenderTextureのpsoName
	std::string psoNameRenderTexture_ = "Fullscreen";
	// RTVのインデックス
	uint32_t rtvIndex_;
	// RenderTextureのsrvIndex
	uint32_t renderTextureSRVIndex_;

};

