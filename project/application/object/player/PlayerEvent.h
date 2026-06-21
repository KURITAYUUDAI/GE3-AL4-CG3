#pragma once

struct PlayerHPChangeEvent
{
	int currentHitPoint = 0;
	int previousHitPoint = 1;
	int maxHitPoint = 1;
};

struct PlayerWorldPositionEvent
{
	Vector3 worldPosition{};
};

//struct PlayerRockOnEvent
//{
//	EnemyID enemyID;
//	Vector3 enemyPositionEvent{};
//};