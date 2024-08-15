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

struct Combination
{
	uint8_t numberOfCards{};
	uint8_t power{};

	CombinationType combinationType{};

private:
	Combination() = default;
	friend class Tichu;
};

class Tichu final
{
public:
	//To be used to check combinations etc

	//Expects the array is sorted by power level
	static Combination CreateCombination(const std::vector<Card>& cards);
};

