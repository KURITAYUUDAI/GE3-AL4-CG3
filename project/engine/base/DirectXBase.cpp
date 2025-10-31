#include "DirectXBase.h"
#include <format>
#include <cassert>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include "Logger.h"
#include "StringUtility.h"

using namespace Logger;
using namespace StringUtility;

void DirectXBase::Initialize(WindowsAPI* winAPI)
{
	// NULL検出
	assert(winAPI);

	this->winAPI_ = winAPI;
	
	// FPS固定初期化
	fixFPS_->InitializeFixFPS(60.0f);

	// デバイス初期化
	InitializeDevice();
	
	// コマンド関連初期化
	InitializeCommand();

	// スワップチェーン初期化
	InitializeSwapChain(winAPI);

	// 深度バッファの生成
	CreateDepthBuffer();

	// 各種デスクリプタヒープの生成
	CreateDescriptorHeaps();

	// レンダーターゲットビューの初期化
	InitializeRenderTargetView();

	// 深度ステンシルビューの初期化
	InitializeDepthStencilView();

	// フェンスの生成
	CreateFence();

	// ビューポート矩形の初期化
	InitializeViewportRect();

	// シザー矩形の初期化
	InitializeScissorRect();

	// DXCコンパイラの生成
	CreateDXCCompiler();

	// ImGuiの初期化
	InitializeImGui(winAPI);

}

void DirectXBase::Update()
{

}

void DirectXBase::Finalize()
{
	delete fixFPS_;
}

void DirectXBase::PreDraw()
{
	// これから書き込むバックバッファのインデックスを取得
	UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();

	// 今回のバリアはTransition
	barrier_.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	// Noneにしておく
	barrier_.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	// バリアを張る対象のリソース。現在のバックバッファに対して行う
	barrier_.Transition.pResource = swapChainResources_[backBufferIndex].Get();
	// 遷移前（現在）のResourceState
	barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	// 遷移後のResourceState
	barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	// TransitionBarrierを張る
	commandList_->ResourceBarrier(1, &barrier_);


	// 描画先のRTVとDSVを設定する
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	commandList_->OMSetRenderTargets(1, &rtvHandles_[backBufferIndex], false, &dsvHandle);


	// 指定した深度で画面全体をクリアする
	commandList_->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);



	// 指定した色で画面全体をクリアする
	float clearColor[] = { 0.1f, 0.25f, 0.5f, 1.0f };	// 青っぽい色、RGBAの順
	commandList_->ClearRenderTargetView(rtvHandles_[backBufferIndex], clearColor, 0, nullptr);





	// 描画用のDescriptorHeapの設定
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeaps[] = { srvDescriptorHeap_ };
	commandList_->SetDescriptorHeaps(1, descriptorHeaps->GetAddressOf());

	// 02_00
	commandList_->RSSetViewports(1, &viewport_); // Viewportを設定
	commandList_->RSSetScissorRects(1, &scissorRect_);	// Scissorを設定

}

void DirectXBase::PostDraw()
{
	HRESULT hr;

	UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();

	// 画面に描く処理はすべて終わり、画面に映すので、状態を遷移
	// 今回はRenderTargetからPresentにする
	barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	// TransitionBarrierを張る
	commandList_->ResourceBarrier(1, &barrier_);

	// コマンドリストの内容を確定させる。すべてのコマンドを積んでからCloseすること
	hr = commandList_->Close();
	assert(SUCCEEDED(hr));

	// FPS固定
	fixFPS_->UpdateFixFPS();

	// GPUにコマンドリストの実行を行わせる
	Microsoft::WRL::ComPtr<ID3D12CommandList> commandLists[] = { commandList_ };
	commandQueue_->ExecuteCommandLists(1, commandLists->GetAddressOf());
	// GPUとOSに画面の交換を行うよう通知する
	swapChain_->Present(1, 0);

	// Fenceの値を更新
	fenceValue_++;
	// GPUがここまでたどり着いたときに、Fenceの値に代入するようにSignalを送る
	commandQueue_->Signal(fence_.Get(), fenceValue_);

	// Fenceの値が指定したSignal値にたどり着いているか確認する
	// GetCompleteValueの初期値はFence作成時に渡した初期値
	if (fence_->GetCompletedValue() < fenceValue_)
	{
		// 指定したSignalにたどりついていないので、たどり着くまで待つようにイベントを設定する
		fence_->SetEventOnCompletion(fenceValue_, fenceEvent_);
		// イベント待つ
		WaitForSingleObject(fenceEvent_, INFINITE);
	}

	// 次のフレーム用のコマンドリストを準備
	hr = commandAllocator_->Reset();
	assert(SUCCEEDED(hr));
	hr = commandList_->Reset(commandAllocator_.Get(), nullptr);
	assert(SUCCEEDED(hr));

}

Microsoft::WRL::ComPtr<IDxcBlob> DirectXBase::CompileShader(const std::wstring& filePath, const wchar_t* profile)
{
	/// 1. hlslファイルを読む
	// これからシェーダーをコンパイルする旨をログに出す
	Log(ConvertString(std::format(L"Begin CompileShader, path:{}, profile:{}\n", filePath, profile)));
	// hlslファイルを読む
	Microsoft::WRL::ComPtr<IDxcBlobEncoding> shaderSource = nullptr;
	HRESULT hr;
	hr = dxcUtils_->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	// 読めなかったら止める
	assert(SUCCEEDED(hr));
	// 読み込んだファイルの内容を設定する
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;

	/// 2. Compileする
	LPCWSTR arguments[] =
	{
		filePath.c_str(),	// コンパイル対象のhlslファイル名
		L"-E", L"main",		// エントリーポイントの指定。基本的にmain文以外にはしない
		L"-T", profile,		// ShaderProfileの指定
		L"-Zi", L"-Qembed_debug",	// デバッグ用の情報を埋め込む
		L"-Od",		// 最適化を外しておく
		L"-Zpr",	// メモリレイアウトは行優先
	};
	// 実際にShaderをコンパイルする
	Microsoft::WRL::ComPtr<IDxcResult> shaderResult = nullptr;
	hr = dxcCompiler_->Compile(
		&shaderSourceBuffer,	// 読み込んだファイル
		arguments,				// コンパイルオプション
		_countof(arguments),	// コンパイルオプションの数
		includeHandler_.Get(),			// includeが含まれた諸々
		IID_PPV_ARGS(&shaderResult) // コンパイル結果
	);
	// コンパイルエラーではなくdxcが起動できないなど致命的な状況
	assert(SUCCEEDED(hr));

	// 3. 警告・エラーがでてないか確認する
	Microsoft::WRL::ComPtr<IDxcBlobUtf8> shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0)
	{
		Log(shaderError->GetStringPointer());
		// 警告・エラーダメゼッタイ
		assert(false);
	}

	// 4. Compile結果を受け取って返す
	// コンパイル結果から実行用のバイナリ部分を取得
	Microsoft::WRL::ComPtr<IDxcBlob> shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));
	// 成功したログを出す
	Log(ConvertString(std::format(L"Compile Succeeded, path:{}, profile:{}\n", filePath, profile)));

	// 実行用のバイナリを返却
	return shaderBlob;
}

Microsoft::WRL::ComPtr<ID3D12Resource> DirectXBase::CreateBufferResource(size_t sizeInBytes)
{
	// ===== [テキストより独自で変換したポイント] =======
	// Vector4 → XMFLOAT4
	//                                         by ChatGPT
	// ==================================================

	// リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;	// UploadHeapを使う
	// リソースの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	// バッファリソース。テクスチャの場合はまた別の設定をする
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Width = sizeInBytes;	// リソースのサイズ。引数より代入
	// バッファの場合はこれらは1にする決まり
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = 1;
	// バッファの場合はこれにする決まり
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	// 実際にリソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
	HRESULT hr;
	hr = device_->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
		&resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&resource));

	/*if (FAILED(hr)) {
		char msg[256];
		sprintf_s(msg, "CreateCommittedResource failed: hr=0x%08X\n", hr);
		OutputDebugStringA(msg);
	}*/

	// リソースが作れなかったので起動できない
	assert(SUCCEEDED(hr));
	return resource;
}

DirectX::ScratchImage DirectXBase::LoadTexture(const std::string& filePath)
{
	// テクスチャファイルを呼んでプログラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = StringUtility::ConvertString(filePath);
	// DirectX::WIC_FLAGS_FORCE_SRGB : sRGB空間で作られたモノとして読む。
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	// ミップマップの作成　（MipMap : 元画像より小さなテクスチャ群）
	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(hr));

	// ミップマップ付きのデータを返す
	return mipImages;
}

Microsoft::WRL::ComPtr<ID3D12Resource> DirectXBase::CreateTextureResource(const DirectX::TexMetadata& metadata)
{
	// 1. metadataを基にResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width); // Textureの幅
	resourceDesc.Height = UINT(metadata.height); // Textureの高さ
	resourceDesc.MipLevels = UINT16(metadata.mipLevels); // mipmapの数
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize); // 奥行き　or　配列Textureの配列数
	resourceDesc.Format = metadata.format; // サンプリングカウント。1固定。
	resourceDesc.SampleDesc.Count = 1; // サンプリングカウント。1固定。
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension); // テクスチャの次元数。普段使っているのは2次元

	// 2. 利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_CUSTOM;	// 細かい設定を行う
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK; // WriteBackポリシーでCPUアクセス可能
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0; // プロセッサの近くに配置

	// 3. Resourceの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
	HRESULT hr;
	hr = device_->CreateCommittedResource(
		&heapProperties, // Heapの設定
		D3D12_HEAP_FLAG_NONE, // Heapの特殊な設定。特になし。
		&resourceDesc, // Resourceの設定
		D3D12_RESOURCE_STATE_GENERIC_READ, // 初回のResourceState。Textureは基本読むだけ
		nullptr, // Clear最適値。使わないのでnullptr
		IID_PPV_ARGS(&resource)); // 作成するResourceポインタへのポインタ
	assert(SUCCEEDED(hr));
	return resource;
}

void DirectXBase::UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages)
{
	// Meta情報を取得
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	// 全MipMapについて
	for (size_t mipLevel = 0; mipLevel < metadata.mipLevels; ++mipLevel)
	{
		// MipMapLevelを指定して各Imageを取得
		const DirectX::Image* img = mipImages.GetImage(mipLevel, 0, 0);
		// Textureに転送
		HRESULT hr = texture->WriteToSubresource(
			UINT(mipLevel),
			nullptr,				// 全領域へコピー
			img->pixels,			// 元データアドレス
			UINT(img->rowPitch),	// 1ラインサイズ
			UINT(img->slicePitch)	// 1枚サイズ
		);
		assert(SUCCEEDED(hr));
	}
}



void DirectXBase::InitializeDevice()
{
	HRESULT hr;

	// HRESULTはWindows系のエラーコードであり、
	// 関数が成功したかどうかをSUCCEEDEDマクロで判定できる
	hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
	// 初期化の根本的な部分でエラーが出た場合はプログラムが間違っているか、どうにも出来ない場合が多いのでassertにしておく
	assert(SUCCEEDED(hr));

	// 使用するアダプタ用の変数。最初にnullptrを入れておく
	Microsoft::WRL::ComPtr<IDXGIAdapter4> useAdapter = nullptr;
	// 良い順にアダプタを頼む
	for (UINT i = 0; dxgiFactory_->EnumAdapterByGpuPreference(i,
		DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) !=
		DXGI_ERROR_NOT_FOUND; ++i)
	{
		// アダプターの情報を取得する
		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr = useAdapter->GetDesc3(&adapterDesc);
		assert(SUCCEEDED(hr)); // 取得できないのは一大事
		// ソフトウェアアダプタでなければ採用！
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE))
		{
			// 採用したアダプタの情報をログに出力、wstringの方なので注意
			Log(ConvertString(std::format(L"Use Adapter:{}\n", adapterDesc.Description)));
			break;
		}
		useAdapter = nullptr;
	}
	// 適切なアダプタが見つからなかったので起動できない
	assert(useAdapter != nullptr);


	// 機能レベルとログ出力用の文字列
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_12_2, D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0
	};

	const char* featureLevelStrings[] = { "12.2", "12.1", "12.0" };
	// 高い順に生成できるか試していく
	for (size_t i = 0; i < _countof(featureLevels); ++i)
	{
		// 採用したアダプターでデバイスを生成
		hr = D3D12CreateDevice(useAdapter.Get(), featureLevels[i], IID_PPV_ARGS(&device_));
		// 指定したアダプターでデバイスが生成できたかを確認
		if (SUCCEEDED(hr))
		{
			// 生成できたのでログ出力を行ってループを抜ける
			Log(std::format("FeatureLevel : {}\n", featureLevelStrings[i]));
			break;
		}
	}

	// デバイスの生成がうまくいかなかったので起動できない
	assert(device_ != nullptr);
	Log("Complete create D3D12Device!!!\n");// 初期化完了のログを出す

}

void DirectXBase::InitializeCommand()
{
	HRESULT hr;

	// コマンドアロケータを生成する
	hr = device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator_));
	// コマンドアロケータの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr));

	// コマンドリストを生成する
	
	hr = device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(), nullptr,
		IID_PPV_ARGS(&commandList_));
	// コマンドリストの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr));

	// コマンドキューを生成する
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	hr = device_->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue_));
	// コマンドキューの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr));

}

void DirectXBase::InitializeSwapChain(WindowsAPI* winAPI)
{
	HRESULT hr;

	// スワップチェーンを生成する
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = WindowsAPI::kClientWidth;		// 画面の幅。ウィンドウのクライアント領域を同じものにしておく
	swapChainDesc.Height = WindowsAPI::kClientHeight;	// 画面の高さ。ウィンドウのクライアント領域を同じものにしておく
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// 色の形式
	swapChainDesc.SampleDesc.Count = 1;	// マルチサンプルしない
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// 描画のターゲットとして利用する
	swapChainDesc.BufferCount = kSwapChainBufferCount;	// ダブルバッファ
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;	// モニタにうつしたら、中身を破棄
	// コマンドキュー、ウィンドウハンドル、設定を渡して生成する
	hr = dxgiFactory_->CreateSwapChainForHwnd(
		commandQueue_.Get(),
		winAPI_->GetHwnd(),
		&swapChainDesc,
		nullptr,
		nullptr,
		reinterpret_cast<IDXGISwapChain1**>(swapChain_.GetAddressOf()));
	assert(SUCCEEDED(hr));

}

void DirectXBase::CreateDepthBuffer()
{
	// 生成するResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = WindowsAPI::kClientWidth;	// Textureの幅
	resourceDesc.Height = WindowsAPI::kClientHeight;	// Textureの高さ
	resourceDesc.MipLevels = 1;	// mipmapの数
	resourceDesc.DepthOrArraySize = 1;	// 奥行き or 配列Textureの配列数
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	// DepthStencilとして利用可能なフォーマット
	resourceDesc.SampleDesc.Count = 1;	// サンプリングカウント。1固定。
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;	// 2次元
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;	// DepthStencilとして使う通知

	// 利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;	// VRAM上に作る

	// 深度値のクリア設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;	// 1.0f（最大値）でクリア
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	// フォーマット。Resourceと合わせる

	// Resourceの生成

	HRESULT hr = device_->CreateCommittedResource(
		&heapProperties,	// Heapの設定
		D3D12_HEAP_FLAG_NONE,	// Heapの特殊な設定。特になし。
		&resourceDesc,	// Resourceの設定
		D3D12_RESOURCE_STATE_DEPTH_WRITE,	// 深度値を書き込む状態にしておく
		&depthClearValue,	// Clear最適値
		IID_PPV_ARGS(&depthResource_)
	);
	assert(SUCCEEDED(hr));
}

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> 
DirectXBase::CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible)
{
	// ディスクリプタ―ヒープの生成
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = heapType;
	descriptorHeapDesc.NumDescriptors = numDescriptors;
	descriptorHeapDesc.Flags = 
		shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HRESULT hr = device_->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));

	// ディスクリプタヒープが作れなかったので起動できない
	assert(SUCCEEDED(hr));
	return descriptorHeap;
}

void DirectXBase::CreateDescriptorHeaps()
{

	// RTV用のヒープでディスクリプタの数は2、RTVはShader内で触るものではないので、ShaderVisibleはfalse
	rtvDescriptorHeap_ = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);

	// SRV用のヒープでディスクリプタの数は128、SRVはShader内で触るものなので、ShaderVisibleはtrue
	srvDescriptorHeap_ = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);

	// DSV用のヒープディスクリプタの数は1。DSVはShader内で飾るものではないので、ShaderVisibleはfalse
	dsvDescriptorHeap_ = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);

	rtvDescriptorSize_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	srvDescriptorSize_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	dsvDescriptorSize_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	
}

D3D12_CPU_DESCRIPTOR_HANDLE DirectXBase::GetCPUDescriptorHandle(
	const ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize, uint32_t index)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize * index);
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE DirectXBase::GetGPUDescriptorHandle(
	const ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize, uint32_t index)
{
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriptorSize * index);
	return handleGPU;
}

void DirectXBase::InitializeRenderTargetView()
{
	HRESULT hr;
	
	hr = swapChain_->GetBuffer(0, IID_PPV_ARGS(&swapChainResources_[0]));
	// うまく取得できなければ起動できない
	assert(SUCCEEDED(hr));
	hr = swapChain_->GetBuffer(1, IID_PPV_ARGS(&swapChainResources_[1]));
	assert(SUCCEEDED(hr));

	// RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = kRtvFormat;	// 出力結果をSRGBに変換して書き込む
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;	// 2Dテクスチャとして書き込む
	
	for (int32_t i = 0; i < 2; i++)
	{
		rtvHandles_[i] = GetCPUDescriptorHandle(
		rtvDescriptorHeap_,
		device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV), i);
		device_->CreateRenderTargetView(swapChainResources_[i].Get(), &rtvDesc, rtvHandles_[i]);
	}
}

D3D12_CPU_DESCRIPTOR_HANDLE DirectXBase::GetRTVCPUDescriptorHandle(uint32_t index)
{
	return GetCPUDescriptorHandle(
		rtvDescriptorHeap_, rtvDescriptorSize_, index);
}

D3D12_GPU_DESCRIPTOR_HANDLE DirectXBase::GetRTVGPUDescriptorHandle(uint32_t index)
{
	return GetGPUDescriptorHandle(
		rtvDescriptorHeap_, rtvDescriptorSize_, index);
}

D3D12_CPU_DESCRIPTOR_HANDLE DirectXBase::GetSRVCPUDescriptorHandle(uint32_t index)
{
	return GetCPUDescriptorHandle(
		srvDescriptorHeap_, srvDescriptorSize_, index);
}

D3D12_GPU_DESCRIPTOR_HANDLE DirectXBase::GetSRVGPUDescriptorHandle(uint32_t index)
{
	return GetGPUDescriptorHandle(
		srvDescriptorHeap_, srvDescriptorSize_, index);
}

D3D12_CPU_DESCRIPTOR_HANDLE DirectXBase::GetDSVCPUDescriptorHandle(uint32_t index)
{
	return GetCPUDescriptorHandle(
		dsvDescriptorHeap_, dsvDescriptorSize_, index);
}

D3D12_GPU_DESCRIPTOR_HANDLE DirectXBase::GetDSVGPUDescriptorHandle(uint32_t index)
{
	return GetGPUDescriptorHandle(
		dsvDescriptorHeap_, dsvDescriptorSize_, index);
}

void DirectXBase::InitializeDepthStencilView()
{
	// DSVの設定
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	// Format。基本的にはResourceに合わせる
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;	// 2dTexture
	
	// DSVHeapの先頭にDSVをつくる
	device_->CreateDepthStencilView(
		depthResource_.Get(), 
		&dsvDesc, 
		dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart());
}

void DirectXBase::CreateFence()
{
	HRESULT hr;

	hr = device_->CreateFence(fenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
	assert(SUCCEEDED(hr));

	// FenceのSignalを待つためのイベントを作成する
	fenceEvent_ = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent_ != nullptr);
}

void DirectXBase::InitializeViewportRect()
{
	
	// クライアント領域のサイズと一緒にして画面全体に表示
	viewport_.Width = static_cast<float>(WindowsAPI::kClientWidth);
	viewport_.Height = static_cast<float>(WindowsAPI::kClientHeight);
	viewport_.TopLeftX = 0;
	viewport_.TopLeftY = 0;
	viewport_.MinDepth = 0.0f;
	viewport_.MaxDepth = 1.0f;
}

void DirectXBase::InitializeScissorRect()
{
	// 基本的にビューポートと同じ矩形が構成されるようにする
	scissorRect_.left = 0;
	scissorRect_.right = WindowsAPI::kClientWidth;
	scissorRect_.top = 0;
	scissorRect_.bottom = WindowsAPI::kClientHeight;
}

void DirectXBase::CreateDXCCompiler()
{
	HRESULT hr;

	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));
	assert(SUCCEEDED(hr));

	hr = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
	assert(SUCCEEDED(hr));
}

void DirectXBase::InitializeImGui(WindowsAPI* winAPI)
{
	// ImGuiの初期化。詳細は省かれたが「こういうもん」らしい。
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(winAPI->GetHwnd());
	ImGui_ImplDX12_Init(device_.Get(),
		kSwapChainBufferCount,
		kRtvFormat,
		srvDescriptorHeap_.Get(),
		srvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart(),
		srvDescriptorHeap_->GetGPUDescriptorHandleForHeapStart());

}