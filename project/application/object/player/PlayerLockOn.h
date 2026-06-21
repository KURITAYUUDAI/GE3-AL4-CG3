#pragma once
#include "GamePlayUIUtility.h"
#include "enemy/EnemyEvent.h"
#include "PlayerEvent.h"
#include "enemy/EnemyManager.h"

class PlayerLockOn
{
public:
    
    void Update(const Vector3& playerPosition)
    {
        if (!enemyManager_)
        {
            lockOnEnemyID_ = 0;
            return;
        }

        lockOnEnemyID_ =
            enemyManager_->FindNearestEnemyID(playerPosition);
    }

 

    EnemyID GetLockOnEnemyID() const
    {
        return lockOnEnemyID_;
    }

    Vector3 GetLockOnPosition()
    {
        return enemyManager_->FindEnemy(lockOnEnemyID_)->GetWorldPosition();
    }

private:
    EnemyManager* enemyManager_ = EnemyManager::GetInstance();
    EnemyID lockOnEnemyID_ = 0;
};

