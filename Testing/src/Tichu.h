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
	CT_FullHouse = 6,

	CT_Dogs = 7,
	CT_SinglePhoenix = 8,

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

	CombinationType combinationType{CombinationType::CT_Invalid};

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
	Combination CreateCombination(const std::vector<Card>& cards) const;
	bool CanFulfillWish(const uint8_t wishPower, const std::vector<Card>& cards) const;

	//Returns if the combination was accepted and taken in as the highest combination played
	bool PlayHand(const Combination combination);

	void UpdatePlayerStates(const std::vector<PlayerState>& playerStates);

	void SetStartingPlayer(const int startingPlayerIndex);


	int CountPoints(const std::vector<Card>& cards) const;

	//First bool is if the pass is valid, second is if it's a round reset, meaning all players left passed in a row
	std::pair<bool, bool> Pass();
	const int& GetCurrentPlayerIndex() const { return m_CurrentPlayerIndex; }
	int GetPreviousPlayerIndex() const;
	int GetPlayersLeftWhenLastHandPlayed() const { return m_PlayersLeftWhenLastHandPlayed; }

private:
	Combination m_CurrentStrongestCombination{};

	void NextPlayer();
	void CountPlayersLeft();

	int m_PlayersLeftWhenLastHandPlayed{ 4 };
	int m_PassesInARow{0};

	int m_CurrentPlayerIndex{};
	std::vector<PlayerState> m_PlayerStates{};
};