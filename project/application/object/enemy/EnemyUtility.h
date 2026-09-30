#pragma once
#include <cstdint>

using EnemyID = std::uint32_t;

enum class EnemyBattlePhase
{
	MoveToRanged,
	Ranged,
	Approach,
	CloseWait,
	CloseAttack,
	Recovery,
};
