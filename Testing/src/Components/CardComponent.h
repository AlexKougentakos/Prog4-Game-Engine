#pragma once
#include "Component.h"

enum CardColour : uint8_t;

class CardComponent final : public ody::Component
{
public:
	explicit CardComponent(CardColour colour, uint8_t power);
	
	~CardComponent() override = default;
	CardComponent(const CardComponent& other) = delete;
	CardComponent(CardComponent&& other) = delete;
	CardComponent& operator=(const CardComponent& other) = delete;
	CardComponent& operator=(CardComponent&& other) = delete;

	void Initialize() override;

private:
	CardColour m_Colour{};
	uint8_t m_Power{};
};
