#pragma once
#include <vector>
#include <glm/vec3.hpp>

#include "Component.h"

namespace ody
{
	class Texture2D;
}

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

	void LoadCardTextures();
	void CalculateRenderingParameters();
};
