#pragma once
#include <cstdint>

enum class UILayer 
{
    World,   // 3D空間上のUI
    Screen,  // 画面固定UI
    Overlay  // 最前面UI
};

struct UIID 
{
    uint32_t value = 0;

    bool operator==(const UIID& other) const
    {
        return value == other.value;
    }

    bool operator!=(const UIID& other) const
    {
        return value != other.value;
    }
};

constexpr UIID kInvalidUIID{ 0 };
