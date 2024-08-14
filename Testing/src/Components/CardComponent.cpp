#include "CardComponent.h"

#include <iostream>


CardComponent::CardComponent(CardColour colour, uint8_t power) :
	   m_Colour(colour),
	   m_Power(power)
{
}

void CardComponent::Initialize()
{
	std::cout << "Card created with colour: " << static_cast<int>(m_Colour) << " and power: " << static_cast<int>(m_Power) << '\n';
}
