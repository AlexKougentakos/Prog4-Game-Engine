#pragma once
#include <vector>
#include "Components/PlayerComponent.h"



enum class CombinationType : uint8_t
{
	CT_Invalid = 0,
	CT_Single = 1,
	CT_Doubles = 2,
	CT_Triples = 3,
	CT_Straight = 4,
	CT_Steps = 5,
	CT_FullHouse = 6

	//TODO: Add bombs later
};

struct PlayerState
{
	bool declaredTichu{ false };
	bool declaredGrandTichu{ false };
	bool isOut{ false };
};

struct Combination
{
	uint8_t numberOfCards{};
	uint8_t power{};

	CombinationType combinationType{};

private:
	Combination() = default;
	friend class Tichu;
};

struct Card;

class Tichu final
{
public:
	Tichu() = default;
	//Expects the array is sorted by power level
	static Combination CreateCombination(const std::vector<Card>& cards);

	//Returns if the combination was accepted and taken in as the highest combination played
	bool PlayHand(const Combination combination);

	void UpdatePlayerStates(const std::vector<PlayerState>& playerStates);

	bool Pass();
	const int& GetCurrentPlayerIndex() const { return m_CurrentPlayerIndex; }

private:
	Combination m_CurrentStrongestCombination{};

	void NextPlayer();

	int m_CurrentPlayerIndex{};
	std::vector<PlayerState> m_PlayerStates{};
};