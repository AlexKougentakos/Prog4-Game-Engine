#pragma once
#include "CardRenderPackage.h"

#include <map>
#include <string>
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "Component.h"
#include "Subject.h"

class Tichu;
class TichuScene;


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
	CardColour colour{};
	uint8_t power{};

	uint8_t id{};
	
	Card(CardColour colour, uint8_t power)
		: colour(colour), power(power)
	{
		if (colour == CC_Mahjong)
			id = 200;
		else if (colour == CC_Dragon)
			id = 201;
		else if (colour == CC_Phoenix)
			id = 202;
		else if (colour == CC_Dog)
			id = 203;
		else 
			id = static_cast<uint8_t>(colour) * 13 + power;
	}


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
		if (colour == CC_Phoenix && other.colour == CC_Phoenix)
			return true;
		
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

class PlayerComponent : public ody::Component
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

	virtual void OnGuiMCTS() {}

	void SetCards(const std::vector<Card>& newCards);

	virtual void AskForDragon() {}

	void PlayedSelectedCards();
	void Pass();

	void GiveDragon(const int points) { m_HoldingPoints += points; }

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

	void ReportResults(std::ofstream& outFile, std::string filePath) const;

	//ImGui Relative Items
	void ShowCardHitBoxes(const bool show) { m_ShowCardHitboxes = show; }
	void ShowCardBacks(const bool show) { m_ShowCardBacks = show; }

	void SetGameReference(Tichu* game) { m_pTichuGame = game; }
	void SetCardsOnTopReference(const std::vector<Card>* cards) { m_CardsOnTop = *cards; }
	void SetSceneReference(TichuScene* scene);

	virtual float GetDesiredGuiHeight() const { return 120.f; }  // Default height
	virtual bool IsAIPlayer() const { return false; }  // Base class returns false

protected:
	Tichu* m_pTichuGame{};
	TichuScene* m_pScene{};

	ody::Subject m_PlayerSubject{};

	int m_PlayerID{};
	std::vector<Card> m_Cards{};
	std::vector<Card> m_SelectedCards{};

	int m_HoldingPoints{0};
	bool m_DeclaredTichu{ false };
	bool m_DeclaredGrandTichu{ false };

	//todo: try to move this to the human player
	bool m_HitBoxesDirty{ false };
	std::map<Card, CardHitbox, CardMapComparator> m_CardHitBoxMap{};

		CardRenderPackage m_RenderPackage{};

	bool m_ShowMahjongSelectionTable{ false };

	// Cache these values
	float m_ScreenWidth{};
	float m_ScreenHeight{};
	glm::vec3 m_StartPosition{};
	glm::vec3 m_Offset{};
	float m_Rotation{};
	glm::vec2 m_CardPickupDirection{ 0,0 };
	const float m_CardPickupAmount{ 20.f };

	bool m_IsPlaying{ false };
	bool m_IsOut{false};

	//todo: temporary for the AI testing
	std::vector<Card> m_CardsOnTop; // Reference to cards on top

	// ===============
	// Info collection
	// ===============

	//A map that stores how long each move took. This data will be normalized later
	// First int is the round number, second int is the move number and the float is the time it took to calculate the move
	std::map<int, std::map<int, double>> m_RoundMoveTimeMap{};
	int m_RoundCounter{};
	int m_MoveCounter{};
private:

	std::shared_ptr<ody::Texture2D> m_pRedLightTexture{};
	std::shared_ptr<ody::Texture2D> m_pGreenLightTexture{};
	std::shared_ptr<ody::Texture2D> m_pTichuTokenTexture{};
	std::shared_ptr<ody::Texture2D> m_pGrandTichuTokenTexture{};
	glm::vec2 m_LightSize{};
	glm::vec2 m_LightPosition{};


	//On Gui Bindings
	bool m_ShowCardHitboxes{ false };
	bool m_ShowCardBacks{ true };

	void CalculateRenderingParameters();

	void RenderLights() const;
	void RenderCards() const;

};