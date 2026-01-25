#include "InputManager.h"

std::unique_ptr<InputManager, InputManager::Deleter> InputManager::instance_ = nullptr;

InputManager* InputManager::GetInstance()
{
	if (instance_ == nullptr)
	{
		instance_.reset(new InputManager);
	}
	return instance_.get();
}

void InputManager::Finalize()
{
	instance_.reset();
}

void InputManager::Initialize(WindowsAPI* winAPI)
{
	HRESULT hr;

	// ポインタで渡されたWinAPIのインスタンスを記録

	winAPI_ = winAPI;

	// DirectInputのオブジェクトを生成
	hr = DirectInput8Create(
		winAPI_->GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput_, nullptr);
	assert(SUCCEEDED(hr));

	// キーボードデバイスの生成
	hr = directInput_->CreateDevice(GUID_SysKeyboard, &keyboard_, NULL);
	assert(SUCCEEDED(hr));
	// 入力データ形式のセット
	hr = keyboard_->SetDataFormat(&c_dfDIKeyboard);	// 標準形式
	assert(SUCCEEDED(hr));
	// 排他制御レベルのセット
	hr = keyboard_->SetCooperativeLevel(
		winAPI_->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(hr));

	// マウスデバイスの生成
	hr = directInput_->CreateDevice(GUID_SysMouse, &mouse_, NULL);
	assert(SUCCEEDED(hr));
	// 入力データ形式のセット
	hr = mouse_->SetDataFormat(&c_dfDIMouse);	// 標準形式
	assert(SUCCEEDED(hr));
	// 排他制御レベルのセット
	hr = mouse_->SetCooperativeLevel(
		winAPI_->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(hr));
}

void InputManager::Update()
{
	HRESULT hr;

	// 前回のキー入力を保存
	memcpy(keyPre_, key_, sizeof(key_));

	// 前回のマウス入力を保存
	memcpy(&mouseStatePre_, &mouseState_, sizeof(mouseState_));

	// キーボード情報の取得開始
	hr = keyboard_->Acquire();

	// 全キーの入力状態を取得する
	hr = keyboard_->GetDeviceState(sizeof(key_), key_);

	// マウス情報の取得開始
	hr = mouse_->Acquire();

	// マウスカーソルとマウスボタンの入力状態を取得する
	hr = mouse_->GetDeviceState(sizeof(mouseState_), &mouseState_);

}

bool InputManager::PushKey(BYTE keyNum)
{
	if (key_[keyNum])
	{
		return true;
	}
	
	return false;
}

bool InputManager::TriggerKey(BYTE keyNum)
{
	if(!keyPre_[keyNum] && key_[keyNum])
	{
		return true;
	}

	return false;
}

bool InputManager::PushMouse(BYTE mouseButton)
{
	if (mouseState_.rgbButtons[mouseButton] & 0x80)
	{
		return true;
	}
	return false;
}

bool InputManager::TriggerMouse(BYTE mouseButton)
{
	if (!(mouseStatePre_.rgbButtons[mouseButton] & 0x80) 
		 && (mouseState_.rgbButtons[mouseButton] & 0x80))
	{
		return true;
	}
	return false;
}

POINT InputManager::MousePoint()
{
	// マウスのスクリーン座標を取得する
	POINT point;
	GetCursorPos(&point);

	// スクリーン座標をクライアント座標へ変換する
	ScreenToClient(winAPI_->GetHwnd(), &point);

	return point;
}

LONG InputManager::MouseWheel()
{

	LONG wheelData = mouseState_.lZ;

	return wheelData;
}


