#pragma once
#include "InputHandler.h"
#include <memory>

// 接続状況を見てデバイスを自動選択する
class InputHandlerSelector
{
public:
    InputHandlerSelector();

    IInputHandler* GetHandler();

public: // 外部入出力

	GamepadInputHandler* GetGamepadHandler() { return gamepad_.get(); }
	KeyboardInputHandler* GetKeyboardHandler() { return keyboard_.get(); }

private:
    std::unique_ptr<GamepadInputHandler>  gamepad_;
    std::unique_ptr<KeyboardInputHandler> keyboard_;
};
