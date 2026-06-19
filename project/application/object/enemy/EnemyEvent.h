#pragma once
#include "GamePlayUIUtility.h"

struct EnemyAppierEvent
{
    EnemyID enemyID = 0;

    int currentHitPoint = 0;
    int previousHitPoint = 1;
    int maxHitPoint = 1;

    Vector2 screenPosition{};

    bool isVisible = true;

    EnemyHPGageDisplayType displayType =
        EnemyHPGageDisplayType::None;

    int screenBossPriority = 0;
};

struct EnemyScreenPositionEvent
{
    EnemyID enemyID = 0;
    Vector2 screenPosition{};
    bool isVisible = true;
};

struct EnemyHPChangeEvent
{
    EnemyID enemyID = 0;
    int currentHitPoint = 0;
    int previousHitPoint = 1;
    int maxHitPoint = 1;
};
