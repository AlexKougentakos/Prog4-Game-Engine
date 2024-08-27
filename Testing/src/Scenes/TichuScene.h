#pragma once
#include "GameScene.h"

#include <iostream>

#include "Tichu.h"
#include "CardRenderPackage.h"

namespace ody
{
	class TextComponent;
}

enum class GamePhase
{
	GrandTichu,
	TradeCards, //todo
	Playing
};

class Button;
class ButtonManagerComponent;
class CardComponent;

class TichuScene final : public ody::GameScene
{
public:
	TichuScene() : ody::GameScene(L"Tichu Scene") { Initialize(); }

	TichuScene(const TichuScene& other) = delete;
	TichuScene(TichuScene&& other) noexcept = delete;
	TichuScene& operator=(const TichuScene& other) = delete;
	TichuScene& operator=(TichuScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void PostRender() override;
	void Update() override;
	void OnGUI() override;
private:
	void CreateDeck();
	void CreatePlayers();
	void CreateTradeTable();
	void CreateDragonButtons();
	void CreateActionButtons();
	void CreateMahjongSelectionTable();
	void CreatePointDisplay();
	void CreateCardRenderPackage();

	//One-Two (1-2) is a special case where the two players from each team go out one after the other leaving the other two still in the game.
	//In that case no points are counted and you start a new round. The team who went out gain 200 points.
	void NewRound(bool isOneTwo = false);
	void Pass();
	void CheckSubmittedHand();
	void GiveDragonToPlayer(const int playerID) const;
	void GameOver() const;

	void HandleMahjongTable();
	void UpdateTichuButton() const;
	void UpdatePlayerPoints(int indexOfPlayerNotOut);
	void UpdateLights() const;

	
	void DeclareTichu() const;
	void DeclareGrandTichu();
	void DeclineGrandTichu();
	//The parameter is only used for debugging purposes
	void DealInitialCards(const int numberOfCards = 8);
	void DealRestOfCards();

	void SetAnnouncementText(const std::string& text) const;
	void ShowMahjongSelectionTable(const bool show);
	void ConfirmCardTrades();

	int GetCardTextureIndex(const Card& card) const;

	std::vector<PlayerComponent*> m_pPlayers{};
	std::vector<Card> m_Cards{};
	std::vector<Card> m_CardsOnTop{};
	std::vector<Card> m_PlayedCards{};
	CardRenderPackage m_RenderPackage{};
	std::unique_ptr<Tichu> m_pTichuGame{};

	std::vector<ody::TextComponent*> m_pMahjongButtonTextComponents{};
	ody::TextComponent* m_Team0PointsText{};
	ody::TextComponent* m_Team1PointsText{};
	ody::TextComponent* m_GamesWonCounter{};
	ody::TextComponent* m_pAnnouncementText{};
	std::vector<Button*> m_pMahjongButtons{};
	std::vector<Button*> m_pDragonButtons{};
	std::vector<Button*> m_pTradeTableButtons{};
	std::vector<Button*> m_pTradeTableSelections{};


	std::vector<bool> m_CardsSelectedForTrade{};
	std::vector<int> m_CardsSelectedForTradeIndex{};
	std::vector<Card> m_CardsForTrade{};

	int m_PlayersWhoTradedCards{ 0 };
	int m_CurrentSelectedTradingSlotIndex{-1};


	GamePhase m_GamePhase{ GamePhase::GrandTichu };
	int m_MaxPoints{ 500 };

	void CreateButtonTextAtPosition(const std::string& text, const glm::vec2& position, const glm::vec2& buttonSize);

	int m_NumberOfPlayersOut{0};
	int m_IndexOfFirstPlayerOut{0};
	int m_PlayerWhoThrewDogsIndex{ 0 };
	int m_PlayersAskedForGrandTichu{ 0 };

	int m_Team0Points{ 0 }; //Player 0 and 2 
	int m_Team1Points{ 0 }; //Player 1 and 3
	int m_Team0GamesWon{ 0 };
	int m_Team1GamesWon{ 0 };

	bool m_IsMahjongSelectionTableVisible{ false };
	int m_CurrentMahjongWishPower{0};

	ButtonManagerComponent* m_pButtonManager{};
	Button* m_pPlayButton{};
	Button* m_pPassButton{};
	Button* m_pTichuButton{};
	Button* m_pGrandTichuButton{};
	Button* m_pDealCardsButton{};
	Button* m_pConfirmTradesButton{};

	//ImGui
	bool m_ShowCardHitboxes{false};
	bool m_ShowCardBacks{true};
};
