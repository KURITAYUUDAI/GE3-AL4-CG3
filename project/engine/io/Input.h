#pragma once
#include <Windows.h>
#include <cstdint>
#include <cassert>
#include <wrl.h>

#define DIRECTINPUT_VESION		0x0800	// DirectInputのバージョンを指定
#include <dinput.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

class Input
{
public:
	// namespace省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:

	void Initialize(const HINSTANCE hInstance, const HWND hwnd);

	void Update();

	bool PushKey(BYTE keyNum);
	bool TriggerKey(BYTE keyNum);

	POINT MousePoint(const HWND hwnd);
	bool PushMouse(BYTE mouseButton);
	bool TriggerMouse(BYTE mouseButton);


private:

	// DirecctInputの初期化
	ComPtr<IDirectInput8> directInput_ = nullptr;

	ComPtr<IDirectInputDevice8> keyboard_ = nullptr;
	BYTE key_[256] = { 0 };	// キーの状態。256キー分の状態を保持する
	BYTE keyPre_[256] = { 0 };

	ComPtr<IDirectInputDevice8> mouse_ = nullptr;
	DIMOUSESTATE mouseState_ = { 0 };
	DIMOUSESTATE mouseStatePre_ = { 0 };

};

class GetMouse
{
public:

	void Initialize(const HINSTANCE hInstance, const HWND hwnd);

	void Update(const HWND hwnd);

	POINT GetMousePoint() { return mousePoint_; }
	bool GetMouseButton(const int buttonNum) { return mouseButton_[buttonNum]; }

private:

	IDirectInputDevice8* mouse_ = nullptr;
	BYTE mouseButton_[3] = { 0, 0, 0 };	// 左、中央、右ボタンの状態
	POINT mousePoint_ = { 0, 0 };	// マウスカーソルの位置 = nullptr;
	
};



