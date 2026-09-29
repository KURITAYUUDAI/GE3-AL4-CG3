#pragma once
#include <cstdint>

using EnemyID = std::uint32_t;

enum class EnemyBattlePhase
{
	MoveToRanged,
	Ranged,
	ReturnToIdle,
	Approach,
	CloseWait,
	CloseAttack,
	Recovery,
	Retreat,
};
