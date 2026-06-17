#include "EnemyCommand.h"
#include "Enemy/Enemy.h"
#include "AIHandler.h"

void MoveCommand::Execute(Enemy* enemy)
{
    Vector2 direction = aiHandler_->GetDirection();
    if (direction.x == 0.0f && direction.y == 0.0f)
    {
        enemy->Decelerate(); // 入力なし → 減速
    } 
    else
    {
        enemy->Move(direction.x, direction.y); // 入力あり → 加速
    }
}

void ShotCommand::Execute(Enemy* enemy)
{
    enemy->Shot();
}

void AttackCommand::Execute(Enemy* enemy)
{
    enemy->Attack();
}
