#include "InputHandler.h"

Vector2 KeyboardInputHandler::GetDirection()
{
    float x = 0.0f, y = 0.0f;
    auto* input = InputManager::GetInstance();

    if (input->PushKey(DIK_A)) x -= 1.0f;
    if (input->PushKey(DIK_D)) x += 1.0f;
    if (input->PushKey(DIK_W)) y += 1.0f;
    if (input->PushKey(DIK_S)) y -= 1.0f;

    float magnitude = std::sqrt(x * x + y * y);
    if (magnitude > 1.0f)
    {
        x /= magnitude;
        y /= magnitude;
    }

    return { x, y };
}





Vector2 GamepadInputHandler::GetDirection()
{
    auto* input = InputManager::GetInstance();
    float x = input->GetLeftStickX();
    float y = input->GetLeftStickY();

    float magnitude = std::sqrt(x * x + y * y);
    if (magnitude < deadzone_) return { 0.0f, 0.0f };

    float scale = (magnitude - deadzone_) / (1.0f - deadzone_);
    return { (x / magnitude) * scale, (y / magnitude) * scale };
}

