#include "Tichu.h"

#include <algorithm>
#include <iostream>
#include <unordered_map>

Combination Tichu::CreateCombination(const std::vector<Card>& cards) const
{
	const size_t numberOfCards = cards.size();

	//Determine the combination
	Combination combination{};
	combination.combinationType = CombinationType::CT_Invalid;
	combination.numberOfCards = static_cast<uint8_t>(numberOfCards);

	//Find if the hand contains a phoenix card
	bool hasPhoenix = false;
	bool usedPhoenix = false; //For straights & steps
	for (const Card& card : cards)
	{
		if (card.colour == CardColour::CC_Phoenix)
		{
			hasPhoenix = true;
			break;
		}
	}

	//A single card would always be valid
	if (numberOfCards == 1)
	{
		if (cards[0].colour == CardColour::CC_Dog)
			combination.combinationType = CombinationType::CT_Dogs;
		else
			combination.combinationType = CombinationType::CT_Single;

		if (hasPhoenix)
		{
			combination.combinationType = CombinationType::CT_SinglePhoenix;
			combination.power = m_CurrentStrongestCombination.power + 1;
		}
		else
			combination.power = cards[0].power;

		return combination;
	}
	//Doubles need to be the same
	if (numberOfCards == 2)
	{
		if (cards[0].power == cards[1].power)
		{
			combination.combinationType = CombinationType::CT_Doubles;
			combination.power = cards[0].power;
		}
		
		if (hasPhoenix)
		{
			combination.combinationType = CombinationType::CT_Doubles;
			combination.power = cards[1].power; //1 because the array is sorted
		}

		return combination;
	}
	//Triples also need to be the same
	if (numberOfCards == 3)
	{

		if (cards[0].power == cards[1].power && cards[0].power == cards[2].power)
		{
			combination.combinationType = CombinationType::CT_Triples;
			combination.power = cards[0].power;
		}
		
		if (hasPhoenix && cards[1].power == cards[2].power)
		{
			combination.combinationType = CombinationType::CT_Triples;
			combination.power = cards[1].power;
		}
		return combination;
	}

	//Full House
	if (numberOfCards == 5 )
	{
		if (cards[0].power == cards[1].power && cards[0].power == cards[2].power && cards[3].power == cards[4].power) //In the format 2 2 2 + 3 3 
		{
			combination.combinationType = CombinationType::CT_FullHouse;
			combination.power = cards[0].power;
			return combination;
		}
		else if (cards[0].power == cards[1].power && cards[2].power == cards[3].power && cards[3].power == cards[4].power) // In the format 2 2 + 3 3 3
		{
			combination.combinationType = CombinationType::CT_FullHouse;
			combination.power = cards[4].power;
			return combination;
		}

		if ((hasPhoenix && cards[1].power == cards[2].power && cards[3].power == cards[4].power) ||
			(hasPhoenix && cards[1].power == cards[2].power && cards[2].power == cards[3].power)) //In the format 2 2 2 + 3 3 
		{
			combination.combinationType = CombinationType::CT_FullHouse;
			combination.power = cards[1].power;
			return combination;
		}

		if ((hasPhoenix && cards[2].power == cards[3].power && cards[3].power == cards[4].power) ||
			(hasPhoenix && cards[1].power == cards[2].power && cards[2].power != cards[3].power && cards[3].power == cards[4].power)) // In the format 2 2 + 3 3 3
		{
			combination.combinationType = CombinationType::CT_FullHouse;
			combination.power = cards[3].power;
			return combination;
		}
	}

	//Straight
	if (numberOfCards >= 5)
	{
		bool isStraight = true;
		for (size_t i{}; i < cards.size(); ++i)
		{
			if (hasPhoenix) continue;

			//We reached the final card of the hand
			if (i + 1 >= cards.size())
				break;

			if (cards[i].power + 1 != cards[i + 1].power)
			{	
				if (hasPhoenix && !usedPhoenix && cards[i].power + 2 == cards[i + 1].power)
				{
					i += 1;
					usedPhoenix = true;
					continue;
				}
				isStraight = false;
				break;
			}
		}

		if (isStraight)
		{
			combination.combinationType = CombinationType::CT_Straight;
			combination.power = hasPhoenix ? cards[1].power : cards[0].power;
			return combination;
		}
	}

	//Steps
	if (numberOfCards % 2 == 0 && numberOfCards >= 4)
	{
		bool isSteps = true;
		usedPhoenix = false;
		for (size_t i{}; i < cards.size(); i += 2)
		{
			//We reached the final steps
			if (i + 2 >= cards.size())
				break;

			if (cards[i].power != cards[i + 1].power)
			{
				if (!hasPhoenix || usedPhoenix)
					isSteps = false;
				else
					usedPhoenix = true;
			}

			if (cards[i + 1].power + 1 != cards[i + 3].power)
			{
				isSteps = false;
			}
		}

		if (isSteps)
		{
			combination.combinationType = CombinationType::CT_Steps;
			combination.power = cards[0].power;
			return combination;
		}
	}

	return combination;
}

bool Tichu::CanFulfillWish(const uint8_t wishPower, const std::vector<Card>& cards)
{
	if (m_CurrentStrongestCombination.combinationType == CombinationType::CT_Single || m_CurrentStrongestCombination.combinationType == CombinationType::CT_Invalid)
	{
		//You can't legally play a card of equal or lower power so you can't fulfill the wish
		if (m_CurrentStrongestCombination.power >= wishPower && m_CurrentStrongestCombination.combinationType == CombinationType::CT_Single)
			return false;

		for (const Card& card : cards)
		{
			if (card.power == wishPower)
				return true;
		}
	}

	//For the straight I am not accounting for power, since in order to beat it you need to beat the lowest card.
	//If you are forming a straight with a mahjong the lowest card will always be a 1 and so any other legal straight will beat it
	else if (m_CurrentStrongestCombination.combinationType == CombinationType::CT_Straight)
	{
		//Create a map of the cards and their count
		//This is so that we can easily create a vector of just the powers without duplicates
		std::unordered_map<uint8_t, int> cardCount;
		for (const Card& card : cards)
		{
			cardCount[card.power]++;
		}

		//Create a vector of the unique duplicates
		std::vector<uint8_t> uniquePowers{};
		uniquePowers.reserve(cardCount.size());

		for (const auto& entry : cardCount)
		{
			uniquePowers.emplace_back(entry.first);
		}

		//Sort the array so the powers are in order
		std::sort(uniquePowers.begin(), uniquePowers.end());

		//For every element in the array, start moving forward
		// and see if we can for an array as well as keep track of if it contains the wish
		// if both conditions are true then we are able to fulfill the wish
		for (size_t i = 0; i < uniquePowers.size(); ++i)
		{
			int length = 0;
			bool containsWish = false;

			for (size_t j = i; j < uniquePowers.size(); ++j)
			{
				if (j > i && uniquePowers[j] != uniquePowers[j - 1] + 1)
					break;

				length++;
				if (uniquePowers[j] == wishPower)
				{
					containsWish = true;
				}

				// Check if the current sequence can form a valid straight
				if (containsWish && length >= m_CurrentStrongestCombination.numberOfCards)
				{
					return true;
				}
			}
		}
	}

	return false;
}

bool Tichu::PlayHand(const Combination combination)
{
	if (combination.combinationType == CombinationType::CT_Dogs)
	{
		if (m_CurrentStrongestCombination.combinationType == CombinationType::CT_Invalid)
		{
			m_CurrentPlayerIndex = (m_CurrentPlayerIndex + 2) % 4;
			return true;
		}
		
		return false;
	}

	if (combination.combinationType == CombinationType::CT_SinglePhoenix)
	{
		//Less than 18 so that it can't beat the dragon
		if (m_CurrentStrongestCombination.combinationType == CombinationType::CT_Single && combination.power < 18)
		{
				m_CurrentStrongestCombination = combination;
				NextPlayer();
				m_PassesInARow = 0;
				return true;
		}
		return false;
	}

	if (m_CurrentStrongestCombination.combinationType == CombinationType::CT_SinglePhoenix)
	{
		//>= because the phoenix normally adds +0.5 to the power of the card it was laid on, but I don't use floats
		if (combination.combinationType == CombinationType::CT_Single && combination.power >= m_CurrentStrongestCombination.power)
		{
			m_CurrentStrongestCombination = combination;
			NextPlayer();
			m_PassesInARow = 0;
			return true;
		}
	}

	//The table is empty and your combination is valid
	if (m_CurrentStrongestCombination.numberOfCards == 0 && combination.combinationType != CombinationType::CT_Invalid)
	{
		m_CurrentStrongestCombination = combination;
		NextPlayer();
		m_PassesInARow = 0;
		return true;
	}

	//The combinations are the same but the one thrown is of higher power
	if (m_CurrentStrongestCombination.combinationType == combination.combinationType && 
		combination.numberOfCards == m_CurrentStrongestCombination.numberOfCards &&
		combination.power > m_CurrentStrongestCombination.power )
	{
		m_CurrentStrongestCombination = combination;
		m_PassesInARow = 0;
		NextPlayer();
		return true;
	}

	//The combination cannot beat the one that is already present
	return false;
}

void Tichu::UpdatePlayerStates(const std::vector<PlayerState>& playerStates)
{
	m_PlayerStates = playerStates;
}

void Tichu::SetStartingPlayer(const int startingPlayerIndex)
{
	m_CurrentPlayerIndex = startingPlayerIndex;
}

std::pair<bool, bool> Tichu::Pass()
{
	//If the current highest combination is invalid this means that the table is empty
	//If it's empty, and it's your turn you can't pass
	if (m_CurrentStrongestCombination.combinationType == CombinationType::CT_Invalid) 
		return {false, false};

	++m_PassesInARow;
	bool threePassesInRow = false;
	if (m_PassesInARow >= 3)
	{
		m_PassesInARow = 0;
		threePassesInRow = true;
		m_CurrentStrongestCombination = Combination{};
	}

	NextPlayer();
	return { true, threePassesInRow};
}


void Tichu::NextPlayer()
{
	//Increment the player ID
	do
	{
		m_CurrentPlayerIndex = (m_CurrentPlayerIndex + 1) % 4;
	} while (m_PlayerStates[m_CurrentPlayerIndex].isOut);

	//Keep incrementing until you reach the player's ID who is not out
}