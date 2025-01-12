﻿#pragma once
#include "CardRenderPackage.h"

#include <map>
#include <string>
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "Component.h"


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

	//Store them in the colour so that we don't have to add another variable
	CC_Mahjong = 4,
	CC_Dragon = 5,
	CC_Phoenix = 6,
	CC_Dog = 7
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
	explicit PlayerComponent(const int playerID, const CardRenderPackage& renderPackage);
	
	~PlayerComponent() override = default;
	PlayerComponent(const PlayerComponent& other) = delete;
	PlayerComponent(PlayerComponent&& other) = delete;
	PlayerComponent& operator=(const PlayerComponent& other) = delete;
	PlayerComponent& operator=(PlayerComponent&& other) = delete;

	void Initialize() override;
	void Render() const override;
	void Update() override;
	void OnGui() override;

	void SelectCardAtMousePosition(const glm::vec2& mousePos);
	void SetCards(const std::vector<Card>& newCards);

	void PlayedSelectedCards();
	void Pass();

	void GivePoints(const int points) { m_HoldingPoints += points; }
	int GetPoints() const { return m_HoldingPoints; }
	void ResetPoints() { m_HoldingPoints = 0; }

	bool HasDeclaredTichu() const { return m_DeclaredTichu; }
	bool HasDeclaredGrandTichu() const { return m_DeclaredGrandTichu; }
	void DeclareTichu() { m_DeclaredTichu = true; }
	void DeclareGrandTichu() { m_DeclaredGrandTichu = true; }
	void RemoveDeclarations() { m_DeclaredTichu = false; m_DeclaredGrandTichu = false; }

	void SetPlaying(const bool playing) { m_IsPlaying = playing; }

	int GetPlayerID() const { return m_PlayerID; }
	const std::vector<Card>& GetCards() const { return m_Cards; }
	const std::vector<Card>& GetHand() { return m_SelectedCards; }
	void SetOut(const bool out) { m_IsOut = out; }
	bool IsOut() const { return m_IsOut; }
	bool GetShowMahjongSelectionTable() const { return m_ShowMahjongSelectionTable; }

	//ImGui Relative Items
	void ShowCardHitBoxes(const bool show) { m_ShowCardHitboxes = show; }
	void ShowCardBacks(const bool show) { m_ShowCardBacks = show; }
private:
	int m_PlayerID{};
	std::vector<Card> m_Cards{};
	std::vector<Card> m_SelectedCards{};
	CardRenderPackage m_RenderPackage{};

	int m_HoldingPoints{0};
	bool m_DeclaredTichu{ false };
	bool m_DeclaredGrandTichu{ false };


	// Cache these values
	float m_ScreenWidth{};
	float m_ScreenHeight{};
	glm::vec3 m_StartPosition{};
	glm::vec3 m_Offset{};
	float m_Rotation{};
	glm::vec2 m_CardPickupDirection{ 0,0 };
	const float m_CardPickupAmount{ 20.f };

	std::map<Card, CardHitbox, CardMapComparator> m_CardHitBoxMap{};
	bool m_HitBoxesDirty{ false };

	bool m_IsPlaying{ false };
	bool m_IsOut{false};

	std::shared_ptr<ody::Texture2D> m_pRedLightTexture{};
	std::shared_ptr<ody::Texture2D> m_pGreenLightTexture{};
	std::shared_ptr<ody::Texture2D> m_pTichuTokenTexture{};
	std::shared_ptr<ody::Texture2D> m_pGrandTichuTokenTexture{};
	glm::vec2 m_LightSize{};
	glm::vec2 m_LightPosition{};

	bool m_ShowMahjongSelectionTable{ false };

	//On Gui Bindings
	bool m_ShowCardHitboxes{ false };
	bool m_ShowCardBacks{ true };

	void CalculateRenderingParameters();
	CardHitbox CalculateRotatedHitbox(float centerX, float centerY, float width, float height, float rotation, bool manualCorrection);
	void CalculateHitBoxes();

	void RenderLights() const;
	void RenderCards() const;
};
