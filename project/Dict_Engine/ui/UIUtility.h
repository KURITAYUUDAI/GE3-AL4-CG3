#pragma once

enum class UILayer 
{
    World,   // 3D空間上のUI
    Screen,  // 画面固定UI
    Overlay  // 最前面UI
};

struct GameUIViewModel 
{
    int playerHP = 0;
    int playerMaxHP = 1;
};