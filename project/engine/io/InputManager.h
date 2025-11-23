#pragma once
#include <Windows.h>
#include <cstdint>
#include <cassert>
#include <wrl.h>

#include "WindowsAPI.h"

#define DIRECTINPUT_VESION		0x0800	// DirectInputのバージョンを指定
#include <dinput.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

class InputManager
{
public:
	// namespace省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public: 
	// シングルトンインスタンスの取得
	static InputManager* GetInstance();
	// 終了
	void Finalize();

public:

	void Initialize(WindowsAPI* winAPI);

	void Update();

	bool PushKey(BYTE keyNum);
	bool TriggerKey(BYTE keyNum);

	POINT MousePoint(const HWND hwnd);
	bool PushMouse(BYTE mouseButton);
	bool TriggerMouse(BYTE mouseButton);

private: 	// シングルトンインスタンス

	static InputManager* instance_;
	InputManager() = default;
	~InputManager() = default;
	InputManager(InputManager&) = delete;
	InputManager& operator=(InputManager&) = delete;

private:

	// WindowsAPI
	WindowsAPI* winAPI_ = nullptr;

	// DirecctInputの初期化
	ComPtr<IDirectInput8> directInput_ = nullptr;

	ComPtr<IDirectInputDevice8> keyboard_ = nullptr;
	BYTE key_[256] = { 0 };	// キーの状態。256キー分の状態を保持する
	BYTE keyPre_[256] = { 0 };

	ComPtr<IDirectInputDevice8> mouse_ = nullptr;
	DIMOUSESTATE mouseState_ = { 0 };
	DIMOUSESTATE mouseStatePre_ = { 0 };

};



