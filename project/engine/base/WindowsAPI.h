#pragma once
#include <Windows.h>
#include <cstdint>

class WindowsAPI
{
public:

	// 初期化
	void Initialize();
	// 終了
	void Finalize();

	// メッセージの処理
	bool ProcessMessage();

	// ゲッター
	HWND GetHwnd() const { return hwnd_; }
	HINSTANCE GetHInstance() const { return wc_.hInstance; }

public:	// 静的関数

	// ウィンドウプロシージャ
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

public:	// 定数

	static const int32_t kClientWidth = 1280;
	static const int32_t kClientHeight = 720;

private:

	HWND hwnd_ = nullptr;
	WNDCLASS wc_{};

};

