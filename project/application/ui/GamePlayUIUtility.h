#pragma once

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
