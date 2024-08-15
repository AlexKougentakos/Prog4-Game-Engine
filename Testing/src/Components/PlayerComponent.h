#pragma once
#include <map>
#include <string>
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


	bool operator<(const Card& other) const
	{
		return power < other.power;
	}
	bool operator>(const Card& other) const
	{
		return power > other.power;
	}
	bool operator==(const Card& other) const
	{
		return colour == other.colour && power == other.power;
	}
};

// Custom comparator for std::map
struct CardMapComparator
{
	bool operator()(const Card& lhs, const Card& rhs) const
	{
		// First compare by colour
		if (lhs.colour != rhs.colour)
			return lhs.colour < rhs.colour;

		// If colours are the same, compare by power
		return lhs.power < rhs.power;
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
	void Update() override;
	void OnGui() override;

	const std::vector<Card>& GetHand() { return m_SelectedCards; }

	//ImGui Relative Items
	void ShowCardHitBoxes(const bool show) { m_ShowCardHitboxes = show; }

private:
	int m_PlayerID{};
	std::vector<Card> m_Cards{};
	std::vector<Card> m_SelectedCards{};
	std::vector<std::shared_ptr<ody::Texture2D>> m_CardTextures{};

	// Cache these values
	float m_ScreenWidth{};
	float m_ScreenHeight{};
	glm::vec3 m_StartPosition{};
	glm::vec3 m_Offset{};
	float m_Rotation{};
	const float m_CardScale{ 1.5f };
	const float m_CardSpacing{ 25.f };
	glm::vec2 m_CardPickupDirection{ 0,0 };
	const float m_CardPickupAmount{ 20.f };

	std::map<Card, CardHitbox, CardMapComparator> m_CardHitBoxMap{};
	bool m_HitBoxesDirty{ true };

	//On Gui Bindings
	bool m_ShowCardHitboxes{ true };

	void LoadCardTextures();
	void CalculateRenderingParameters();
	CardHitbox CalculateRotatedHitbox(float centerX, float centerY, float width, float height, float rotation, bool manualCorrection);
	void CalculateHitBoxes();
};
