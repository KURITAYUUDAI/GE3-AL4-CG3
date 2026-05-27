#pragma once
#include "myMath.h"
#include "InputManager.h"
#include <cmath>

class IInputHandler
{
	public:
	virtual ~IInputHandler() = default;
	virtual Vector2 GetDirection() = 0;
};

class KeyboardInputHandler : public IInputHandler
{
public:
    Vector2 GetDirection() override;
};

class GamepadInputHandler : public IInputHandler
{
public:
    explicit GamepadInputHandler(float deadzone = 0.2f)
        : deadzone_(deadzone) {}

    Vector2 GetDirection() override;

private:
    float deadzone_;
};