#pragma once

enum class UILayer 
{
    World,   // 3D空間上のUI
    Screen,  // 画面固定UI
    Overlay  // 最前面UI
};

struct HPViewModel
{
    int currentHitPoint = 0;
    int previousHitPoint = 1;
    int maxHitPoint = 1;
};

struct GameUIViewModel 
{
    HPViewModel playerHitPoint;
};
