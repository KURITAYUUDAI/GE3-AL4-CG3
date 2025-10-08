#pragma once
#include <Windows.h>
#include <cstdint>
#include <cassert>

#define DIRECTINPUT_VESION		0x0800	// DirectInputのバージョンを指定
#include <dinput.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

class GetKey
{
public:

	void Initialize(const HINSTANCE hInstance, const HWND hwnd);

	void Update();

	bool IsPress(uint8_t keyNum);

private:

	IDirectInputDevice8* keyboard_ = nullptr;
	BYTE key_[256] = { 0 };	// キーの状態。256キー分の状態を保持する

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



