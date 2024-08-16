#include "Tichu.h"

Combination Tichu::CreateCombination(const std::vector<Card>& cards)
{
	const size_t numberOfCards = cards.size();

	//Determine the combination
	Combination combination{};
	combination.combinationType = CombinationType::CT_Invalid;
	combination.numberOfCards = static_cast<uint8_t>(numberOfCards);

	//A single card would always be valid
	if (numberOfCards == 1)
	{
		combination.combinationType = CombinationType::CT_Single;
		combination.power = cards[0].power;
	}
	//Doubles need to be the same
	else if (numberOfCards == 2 && cards[0].power == cards[1].power)
	{
		combination.combinationType = CombinationType::CT_Doubles;
		combination.power = cards[0].power;
	}
	//Triples also need to be the same
	else if (numberOfCards == 3 && cards[0].power == cards[1].power && cards[0].power == cards[2].power)
	{
		combination.combinationType = CombinationType::CT_Triples;
		combination.power = cards[0].power;
	}

	else if (numberOfCards == 5 )
	{
		if (cards[0].power == cards[1].power && cards[0].power == cards[2].power && cards[3].power == cards[4].power) //In the format 2 2 2 + 3 3 
		{
			combination.combinationType = CombinationType::CT_FullHouse;
			combination.power = cards[0].power;
		}
		else if (cards[0].power == cards[1].power && cards[2].power == cards[3].power && cards[3].power == cards[4].power) // In the format 2 2 + 3 3 3
		{
			combination.combinationType = CombinationType::CT_FullHouse;
			combination.power = cards[4].power;
		}
	}

	return combination;
}

bool Tichu::PlayHand(const Combination combination)
{
	//The table is empty and your combination is valid
	if (m_CurrentStrongestCombination.numberOfCards == 0 && combination.combinationType != CombinationType::CT_Invalid)
	{
		m_CurrentStrongestCombination = combination;
		return true;
	}

	//The combinations are the same but the one thrown is of higher power
	if (m_CurrentStrongestCombination.combinationType == combination.combinationType && combination.power > m_CurrentStrongestCombination.power)
	{
		m_CurrentStrongestCombination = combination;
		return true;
	}

	//The combination cannot beat the one that is already present
	return false;
}
