#pragma once
#include <map>
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "Component.h"

namespace ody
{
	class Texture2D;
}

struct CardHitbox
{
	float x{};
	float y{};
	float width{};
	float height{};
};

enum CardColour : uint8_t
{
	CC_Black = 0,
	CC_Green = 1,
	CC_Blue = 2,
	CC_Red = 3,
};

struct Card
{
	CardColour colour;
	uint8_t power;

	// Define the less-than operator in order to use a map for the hitboxes
	bool operator<(const Card& other) const
	{
		// First compare by colour
		if (colour != other.colour)
			return colour < other.colour;

		// If colours are the same, compare by power
		return power < other.power;
	}
};

class PlayerComponent final : public ody::Component
{
public:
	//Pass the cards by reference since there are 14 of them (28 bytes)
	explicit PlayerComponent(const int playerID, const std::vector<Card>& cards);
	
	~PlayerComponent() override = default;
	PlayerComponent(const PlayerComponent& other) = delete;
	PlayerComponent(PlayerComponent&& other) = delete;
	PlayerComponent& operator=(const PlayerComponent& other) = delete;
	PlayerComponent& operator=(PlayerComponent&& other) = delete;

	const std::vector<Card>& GetCards() const { return m_Cards; }
	int GetPlayerID() const { return m_PlayerID; }

	void SelectCardAtMousePosition(const glm::vec2& mousePos);

	void Initialize() override;
	void Render() const override;

private:
	int m_PlayerID{};
	std::vector<Card> m_Cards{};
	std::vector<std::shared_ptr<ody::Texture2D>> m_CardTextures{};

	// Cache these values
	float m_ScreenWidth{};
	float m_ScreenHeight{};
	glm::vec3 m_StartPosition{};
	glm::vec3 m_Offset{};
	float m_Rotation{};
	const float m_CardScale{ 1.5f };
	const float m_CardSpacing{ 25.f };

	std::map<Card, CardHitbox> m_CardHitBoxMap{};


	void LoadCardTextures();
	void CalculateRenderingParameters();
	CardHitbox CalculateRotatedHitbox(float centerX, float centerY, float width, float height, float rotation, bool manualCorrection);
};
