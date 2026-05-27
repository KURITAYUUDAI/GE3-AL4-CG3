#pragma once
#include "InputHandler.h"
#include <memory>

// 接続状況を見てデバイスを自動選択する
class InputHandlerSelector
{
public:
    InputHandlerSelector();

    IInputHandler* GetHandler();

private:
    std::unique_ptr<GamepadInputHandler>  gamepad_;
    std::unique_ptr<KeyboardInputHandler> keyboard_;
};
