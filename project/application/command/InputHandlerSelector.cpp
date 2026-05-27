#include "InputHandlerSelector.h"

InputHandlerSelector::InputHandlerSelector()
{
    gamepad_ = std::make_unique<GamepadInputHandler>();
    keyboard_ = std::make_unique<KeyboardInputHandler>();
}

IInputHandler* InputHandlerSelector::GetHandler()
{
    // コントローラーが接続されていればGamePad優先
    if (InputManager::GetInstance()->IsControllerConnected())
    {
        return gamepad_.get();
    }
    return keyboard_.get();
}
