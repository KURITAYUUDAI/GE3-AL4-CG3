#include "GetKeyMouse.h"



void GetKey::Initialize(const HINSTANCE hInstance, const HWND hwnd)
{
	// DirecctInputの初期化
	IDirectInput8* directInput = nullptr;
	// DirectInputのオブジェクトを生成
	HRESULT hr = DirectInput8Create(
		hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput, nullptr);
	assert(SUCCEEDED(hr));

	// キーボードデバイスの生成
	IDirectInputDevice8* keyboard = nullptr;
	hr = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(hr));
	// 入力データ形式のセット
	hr = keyboard->SetDataFormat(&c_dfDIKeyboard);	// 標準形式
	assert(SUCCEEDED(hr));
	// 排他制御レベルのセット
	hr = keyboard->SetCooperativeLevel(
		hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(hr));

	keyboard_ = keyboard;
}

void GetKey::Update()
{
	// キーボード情報の取得開始
	keyboard_->Acquire();

	// 全キーの入力状態を取得する

	keyboard_->GetDeviceState(sizeof(key_), key_);
}

bool GetKey::IsPress(uint8_t keyNum)
{
	if (key_[keyNum])
	{
		return true;
	}
	else
	{
		return false;
	}
}

void GetMouse::Initialize(const HINSTANCE hInstance, const HWND hwnd)
{
	// DirecctInputの初期化
	IDirectInput8* directInput = nullptr;
	// DirectInputのオブジェクトを生成
	HRESULT hr = DirectInput8Create(
		hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput, nullptr);
	assert(SUCCEEDED(hr));

	// マウスデバイスの生成
	IDirectInputDevice8* mouse = nullptr;
	hr = directInput->CreateDevice(GUID_SysMouse, &mouse, NULL);
	assert(SUCCEEDED(hr));
	// 入力データ形式のセット
	hr = mouse->SetDataFormat(&c_dfDIMouse);	// 標準形式
	assert(SUCCEEDED(hr));
	// 排他制御レベルのセット
	hr = mouse->SetCooperativeLevel(
		hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(hr));

	mouse_ = mouse;
}

void GetMouse::Update(const HWND hwnd)
{
	// マウス情報の取得開始
	mouse_->Acquire();

	// マウスの位置を取得する
	mouse_->GetDeviceState(sizeof(mousePoint_), &mousePoint_);
	// マウスの位置をクライアント座標に変換する
	ScreenToClient(hwnd, &mousePoint_);

	// マウスボタンの入力状態を取得する
	mouse_->GetDeviceState(sizeof(mouseButton_), mouseButton_);
}


