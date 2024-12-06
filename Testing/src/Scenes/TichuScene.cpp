#include "TichuScene.h"

#include <imgui.h>
#include <random>

#include "GameObject.h"
#include "TextureComponent.h"
#include "InputManager2.h"
#include "ImGuiManager.h"
#include "Commands/CardSelectCommand.h"
#include "ButtonManagerComponent.h"
#include "ServiceLocator.h"
#include "TextComponent.h"

#include "Components/Players/AIPlayer.h"
#include "Components/Players/HumanPlayer.h"
#include "Components/Players/AIPlayer_MCTS.h"
#include "Helpers/MCTS.h"

void TichuScene::Initialize()
{
	const auto pLevel = CreateGameObject();
	pLevel->AddComponent<ody::TextureComponent>("cloth.png");

	CreatePointDisplay();

	m_pTichuGame = std::make_unique<Tichu>();
	
	CreateCardRenderPackage();
	CreateDeck();
	CreatePlayers();

	//todo: put this back to 8
	DealInitialCards(14);

	const auto scoreCounter = CreateGameObject();
	m_GamesWonCounter = scoreCounter->AddComponent<ody::TextComponent>("0 - 0", "bonzai.ttf", 40);
	m_GamesWonCounter->SetPosition(static_cast<float>(ody::constants::g_ScreenWidth / 2) - scoreCounter->GetComponent<ody::TextComponent>()->GetTextSize().x / 2, -5);
	
	CreateMahjongSelectionTable();

	const auto pAnnouncementText = CreateGameObject();
	m_pAnnouncementText = pAnnouncementText->AddComponent<ody::TextComponent>("begin", "bonzai.ttf", 30);
	m_pAnnouncementText->SetVisible(false);

	UpdateLights();

	//Temp:
	m_ShowCardBacks = false;
	for (const auto& player : m_pPlayers)
	{
		player->ShowCardBacks(m_ShowCardBacks);
	}

	//Todo: remove this, this is just debugging stuff to skip the trading phase
	m_GamePhase = GamePhase::Playing;
	// Check for Mahjong card
	for (const auto& player : m_pPlayers)
	{
		if (std::find(player->GetCards().begin(), player->GetCards().end(), Card{ CardColour::CC_Mahjong, 1 }) != player->GetCards().end())
		{
			player->SetPlaying(true);
			m_pTichuGame->SetStartingPlayer(player->GetPlayerID());
		}
	}
	UpdateLights();
}

void TichuScene::PostRender() 
{
	const float cardHeight = static_cast<float>(m_RenderPackage.cardTextures[0]->GetSize().y);
	const float cardWidth = static_cast<float>(m_RenderPackage.cardTextures[0]->GetSize().x);
	const float stackWidth = m_RenderPackage.cardSpacing * (m_CardsOnTop.size() - 1) + cardWidth * m_RenderPackage.cardScale;
	const auto startPosition = glm::vec3(ody::constants::g_ScreenWidth / 2.f - stackWidth / 2, ody::constants::g_ScreenHeight / 2.f - (cardHeight * m_RenderPackage.cardScale) / 2.f, 0);

	for (size_t i = 0; i < m_CardsOnTop.size(); ++i)
	{
		//Move them to the side slightly, so you can see all the cards
		const glm::vec3 cardPosition = startPosition + glm::vec3{ m_RenderPackage.cardSpacing, 0, 0 } *static_cast<float>(i);

		// Calculate the name of the card texture
		const size_t textureIndex = [&]() -> size_t
			{
				//This is the order in which they are added inside the CreateDeck() function in the TichuScene
				switch (m_CardsOnTop[i].colour)
				{
				case CC_Dog:
					return 52;
				case CC_Dragon:
					return 53;
				case CC_Phoenix:
					return 54;
				case CC_Mahjong:
					return 55;
				default:
					return m_CardsOnTop[i].colour * 13 + m_CardsOnTop[i].power - 2; //Calculate the texture index using the colour to find the right tile image
				}
			}();


		// Render the texture with rotation
		ody::Renderer::GetInstance().RenderTexture(
			*m_RenderPackage.cardTextures[textureIndex],
			cardPosition.x,
			cardPosition.y,
			cardWidth,
			cardHeight,
			m_RenderPackage.cardScale
		);
	}
}

void TichuScene::Update()
{
	HandleMahjongTable();
	//todo: add a dirty flag for this
	std::vector<PlayerState> playerStates{};

	for (const auto& player : m_pPlayers)
	{
		PlayerState playerState{};
		playerState.isOut = player->IsOut();
		playerStates.emplace_back(playerState);
	}
	m_pTichuGame->UpdatePlayerStates(playerStates);

	if (m_GamePhase == GamePhase::GrandTichu && m_PlayersAskedForGrandTichu >= 4)
	{
        SetPhaseToTradeCards();
    }
	if (m_GamePhase == GamePhase::TradeCards && m_PlayersWhoTradedCards >= 4)
	{
		SetPhaseToPlaying();
	}
}

void TichuScene::SetPhaseToPlaying()
{
		m_GamePhase = GamePhase::Playing;

		const std::vector<int> cardTradeOrder = { 1, 2, 3, 2, 3, 0, 3, 0, 1, 0, 1, 2 };

		for (const auto& player : m_pPlayers)
		{
			const int playerID = player->GetPlayerID();
			auto playerCards = player->GetCards();

			// Cards that need to be removed from this player's hand
			const int cardsToDeleteStartIndex = playerID * 3;

			// Remove the cards that are being traded from this player's hand
			playerCards.erase(std::remove_if(playerCards.begin(), playerCards.end(),
				[this, cardsToDeleteStartIndex](const Card& card) -> bool
				{
					// Check if the current card is one of the cards to be deleted
					return (card == m_CardsForTrade[cardsToDeleteStartIndex] ||
						card == m_CardsForTrade[cardsToDeleteStartIndex + 1] ||
						card == m_CardsForTrade[cardsToDeleteStartIndex + 2]);
				}), playerCards.end());


			// Go through each card that should belong to this player
			for (size_t i = 0; i < cardTradeOrder.size(); ++i)
			{
				if (cardTradeOrder[i] == playerID)
				{
					// Give the card to the player
					playerCards.emplace_back(m_CardsForTrade[i]);
				}
			}

			std::sort(playerCards.begin(), playerCards.end());
			player->SetCards(playerCards);

			// Update player's card collection with the new cards
			player->SetCards(playerCards);

			// Check for Mahjong card
			if (std::find(playerCards.begin(), playerCards.end(), Card{ CardColour::CC_Mahjong, 1 }) != playerCards.end())
			{
				player->SetPlaying(true);
				m_pTichuGame->SetStartingPlayer(player->GetPlayerID());
			}

			UpdateLights();
		}

/* 		m_pConfirmTradesButton->SetVisible(false);
		for (const auto& button : m_pTradeTableButtons)
		{
			button->SetVisible(false);
		}

		for (const auto& button : m_pTradeTableSelections)
		{
			button->SetVisible(false);
		}

		//Reset the tint that was given for illustration during the trading phase
		for (const auto& texture : m_RenderPackage.cardTextures)
		{
			texture->Tint({ 1.f, 1.f, 1.f, 1.f });
		} */
}

void TichuScene::OnNotify(ody::GameEvent event, const ody::EventData& data)
{
	switch (event)
	{
	case ody::GameEvent::PlayCards:
		if (const auto* cardData = dynamic_cast<const ody::CardEventData*>(&data))
		{
			CheckSubmittedHand(cardData->playedCards);
		}
		break;
	case ody::GameEvent::Pass:
		Pass();
		break;
	case ody::GameEvent::DeclareTichu:
		DeclareTichu();
		break;
	case ody::GameEvent::AskForDragon:
		if (const auto* dragonData = dynamic_cast<const ody::DragonEventData*>(&data))
		{
			const int totalPoints = m_pTichuGame->CountPoints(m_PlayedCards);
			m_pPlayers[dragonData->recipientPlayerID]->GiveDragon(totalPoints);

			m_CardsOnTop.clear();
			m_PlayedCards.clear();

/* 			UpdateLights();
			m_pTichuGame->NextPlayer(); */
		}
		break;
	}
}

void TichuScene::SetPhaseToTradeCards()
{
/*     m_GamePhase = GamePhase::TradeCards;
    UpdateTichuButton();
    UpdateLights();
    CreateTradeTable();
    m_pGrandTichuButton->SetVisible(false);
    m_pDealCardsButton->SetVisible(false);
    m_pConfirmTradesButton->SetVisible(true);

    // This is just to get out of the way of the selection card table, since it would overlap with the normal cards
    m_pTichuButton->SetPosition({m_pTichuButton->GetPosition().x, m_pTichuButton->GetPosition().y - 60});
    m_pPlayButton->SetPosition({m_pPlayButton->GetPosition().x, m_pPlayButton->GetPosition().y - 60});
    m_pPassButton->SetPosition({m_pPassButton->GetPosition().x, m_pPassButton->GetPosition().y - 60}); */
}

void TichuScene::CreateDeck()
{
	m_Cards.clear();

	for (int i = 0; i < 52; i++)
	{
		const uint8_t cardValue = static_cast<uint8_t>(i % 13 + 2); //This is because 'i' starts at 0 and the cards start at 2
		const CardColour colour{ static_cast<uint8_t>(i / 13) };
		m_Cards.emplace_back(Card{ colour, cardValue });
	}

 	m_Cards.emplace_back(Card{ CardColour::CC_Dog, 0 });
	m_Cards.emplace_back(Card{ CardColour::CC_Dragon, 20 });
	m_Cards.emplace_back(Card{ CardColour::CC_Phoenix, 0 });
	m_Cards.emplace_back(Card{ CardColour::CC_Mahjong, 1 });

	// Shuffle the deck before distributing
	std::random_device rd;
	std::mt19937 g(rd());

	std::shuffle(m_Cards.begin(), m_Cards.end(), g);
}

void TichuScene::CreatePlayers()
{
	for (int i = 0; i < 4; ++i)
	{
		const auto player = CreateGameObject();

		PlayerComponent* playerComponent = nullptr;
		if (i == 0) //First player is human
		{
			playerComponent = player->AddComponent<HumanPlayer>(i, m_RenderPackage);
		}
		else
		{
			playerComponent = player->AddComponent<AIPlayer_MCTS>(i, m_RenderPackage);
		}
		
		// Give the AI players access to game state
		playerComponent->SetGameReference(m_pTichuGame.get());
		playerComponent->SetCardsOnTopReference(&m_CardsOnTop);
		playerComponent->SetSceneReference(this);
		
		m_pPlayers.emplace_back(playerComponent);
	}
}

void TichuScene::CreateCardRenderPackage()
{
	m_RenderPackage.cardTextures.clear();
	m_RenderPackage.cardScale = 1.5f;
	m_RenderPackage.cardSpacing = 25.f;

	for (int i = 0; i < 52; ++i)
	{
		const std::string cardName = std::to_string(i);
		const std::string paddedCardName = std::string(3 - cardName.length(), '0') + cardName;
		const std::string completePath = "Cards/tile" + paddedCardName + ".png";

		auto texture = ody::ResourceManager::GetInstance().LoadTexture(completePath);
		m_RenderPackage.cardTextures.emplace_back(texture);
		m_RenderPackage.cardTexturePaths.emplace_back(completePath);
	}

 	//Dogs
	std::string completePath = "Cards/Dogs.png";
	auto texture = ody::ResourceManager::GetInstance().LoadTexture(completePath);
	m_RenderPackage.cardTextures.emplace_back(texture);
	m_RenderPackage.cardTexturePaths.emplace_back(completePath);

	//Dragon
	completePath = "Cards/Dragon.png";
	texture = ody::ResourceManager::GetInstance().LoadTexture(completePath);
	m_RenderPackage.cardTextures.emplace_back(texture);
	m_RenderPackage.cardTexturePaths.emplace_back(completePath);

	//Phoenix
	completePath = "Cards/Phoenix.png";
	texture = ody::ResourceManager::GetInstance().LoadTexture(completePath);
	m_RenderPackage.cardTextures.emplace_back(texture);
	m_RenderPackage.cardTexturePaths.emplace_back(completePath);

	//Mahjong
	completePath = "Cards/Mahjong.png";
	texture = ody::ResourceManager::GetInstance().LoadTexture(completePath);
	m_RenderPackage.cardTextures.emplace_back(texture);
	m_RenderPackage.cardTexturePaths.emplace_back(completePath);

	m_RenderPackage.cardBack = ody::ResourceManager::GetInstance().LoadTexture("Cards/back.png");
}

void TichuScene::CreatePointDisplay()
{
	const auto pPointDisplay = CreateGameObject();
	pPointDisplay->AddComponent<ody::TextureComponent>("PointDisplay.png");

	constexpr float pointTextOffsetX = 120.f;
	constexpr float pointTextOffsetY = 15.f;

	auto pPointCounterTextHolder = CreateGameObject();
	m_Team0PointsText = pPointCounterTextHolder->AddComponent<ody::TextComponent>("0", "bonzai.ttf", 50);
	m_Team0PointsText->SetPosition(pointTextOffsetX, pointTextOffsetY);

	const auto pTeam0ScoreTextHolder = CreateGameObject();
	const auto pTeam0ScoreTextComponent = pTeam0ScoreTextHolder->AddComponent<ody::TextComponent>("Team 0 Score:", "bonzai.ttf", 30);
	pTeam0ScoreTextComponent->SetPosition(pointTextOffsetX - pTeam0ScoreTextComponent->GetTextSize().x / 2 + m_Team0PointsText->GetTextSize().x / 2, 0);

	pPointCounterTextHolder = CreateGameObject();
	m_Team1PointsText = pPointCounterTextHolder->AddComponent<ody::TextComponent>("0", "bonzai.ttf", 50);
	m_Team1PointsText->SetPosition(ody::constants::g_ScreenWidth - pointTextOffsetX - m_Team1PointsText->GetTextSize().x, pointTextOffsetY);

	const auto pTeam1ScoreTextHolder = CreateGameObject();
	const auto pTeam1ScoreTextComponent = pTeam1ScoreTextHolder->AddComponent<ody::TextComponent>("Team 1 Score:", "bonzai.ttf", 30);
	pTeam1ScoreTextComponent->SetPosition(ody::constants::g_ScreenWidth - pointTextOffsetX - pTeam1ScoreTextComponent->GetTextSize().x / 2 - m_Team1PointsText->GetTextSize().x / 2, 0);

	m_RenderPackage.pointDisplayHeight = pPointDisplay->GetComponent<ody::TextureComponent>()->GetTextureSize().y;
}

void TichuScene::DealInitialCards(const int numberOfCards)
{
	//Deletes the old cards and creates new ones
	//You can just shuffle the current ones but it's done once per round only
	CreateDeck();

	for (const auto& player : m_pPlayers)
	{
		// Calculate the start and end indices for this player's cards
		const int startIndex = player->GetPlayerID() * numberOfCards;
		const int endIndex = startIndex + numberOfCards;

		// Create a vector of cards for this player
		std::vector<Card> playerCards(m_Cards.begin() + startIndex, m_Cards.begin() + endIndex);
			
		std::sort(playerCards.begin(), playerCards.end());
		player->SetCards(playerCards);
	}
}

void TichuScene::DealRestOfCards() const
{
	if (m_PlayersAskedForGrandTichu >= static_cast<int>(m_pPlayers.size()))
	{
		// All players have been dealt their remaining cards
		return;
	}

	constexpr int totalCards = 14;
	constexpr int initialCards = 8;
	constexpr int remainingCards = totalCards - initialCards;

	const auto& player = m_pPlayers[m_PlayersAskedForGrandTichu];
	const int startIndex = initialCards * 4 + remainingCards * m_PlayersAskedForGrandTichu;
	const int endIndex = startIndex + remainingCards;

	// Get the remaining cards for this player
	std::vector<Card> newCards(m_Cards.begin() + startIndex, m_Cards.begin() + endIndex);

	// Add the new cards to the player's existing hand
	auto playerCards = player->GetCards();
	playerCards.insert(playerCards.end(), newCards.begin(), newCards.end());

	// Sort the entire hand
	std::sort(playerCards.begin(), playerCards.end());
	player->SetCards(playerCards);
}

void TichuScene::CheckSubmittedHand(const std::vector<Card>& hand)
{
	const auto& player = m_pPlayers[m_pTichuGame->GetCurrentPlayerIndex()];
	std::vector<Card> submittedHand = hand;
	std::sort(submittedHand.begin(), submittedHand.end());

	if (m_CurrentMahjongWishPower != 0 &&
		m_pTichuGame->CanFulfillWish(static_cast<uint8_t>(m_CurrentMahjongWishPower), player->GetCards()))
	{
		//If we enter here this means that there IS a wish and the player CAN fulfill it
		if (std::find_if(submittedHand.begin(), submittedHand.end(),
			[this](const Card& card)
			{
				return card.power == m_CurrentMahjongWishPower;
			}) == submittedHand.end())
		{
			//If we enter here the player didn't fulfill the wish
			SetAnnouncementText("You have to fulfill the wish of the card: " + std::to_string(m_CurrentMahjongWishPower));
			return;
		}
		else 
		{
			m_CurrentMahjongWishPower = 0;
		}
	}

	const Combination combination = m_pTichuGame->CreateCombination(submittedHand);
	std::sort(submittedHand.begin(), submittedHand.end());
	if (combination.combinationType == CombinationType::CT_Dogs)
		m_PlayerWhoThrewDogsIndex = m_pTichuGame->GetCurrentPlayerIndex();

	if (m_pTichuGame->PlayHand(combination))
	{
		m_PlayerWhoThrewLastCombinationIndex = player->GetPlayerID();
		if (combination.numberOfCards == 1 &&
			std::find(submittedHand.begin(), submittedHand.end(), Card{ CC_Dragon, 20 }) != submittedHand.end())
		{
			ody::ServiceLocator::GetSoundSystem().PlaySound(7);
		}
		else if (std::find(submittedHand.begin(), submittedHand.end(), Card{ CC_Phoenix, 0 }) != submittedHand.end())
		{
			ody::ServiceLocator::GetSoundSystem().PlaySound(9);
		}
		else
		{
			const int randomNum = rand() % 6 + 1;
			ody::ServiceLocator::GetSoundSystem().PlaySound(randomNum);
		}

		const int previousPlayerIndex = combination.combinationType == CombinationType::CT_Dogs ? m_PlayerWhoThrewDogsIndex : m_pTichuGame->GetPreviousPlayerIndex();

		SetAnnouncementText("Player " + std::to_string(previousPlayerIndex) + " played a hand!");
		
		//Previous player because the current player index gets incremented inside PlayHand()
		m_pPlayers[previousPlayerIndex]->PlayedSelectedCards();

		if (m_pPlayers[previousPlayerIndex]->IsOut())
		{
			++m_NumberOfPlayersOut;

			if (m_NumberOfPlayersOut == 1)
			{
				m_IndexOfFirstPlayerOut = previousPlayerIndex;
			}
			else if (m_NumberOfPlayersOut == 2)
			{
				if ((m_pPlayers[0]->IsOut() && m_pPlayers[2]->IsOut()) ||
					(m_pPlayers[1]->IsOut() && m_pPlayers[3]->IsOut()))
				{
					NewRound(true);
					return;
				}

			}

			else if (m_NumberOfPlayersOut == 3)
			{
				NewRound();
				return;
			}
		}

		m_CardsOnTop = submittedHand;
		m_PlayedCards.insert(m_PlayedCards.end(), submittedHand.begin(), submittedHand.end());

		UpdateLights();
		ShowMahjongSelectionTable(false);
	}
}

void TichuScene::Pass()
{
	const auto playerWhoPassed = m_pPlayers[m_pTichuGame->GetCurrentPlayerIndex()];
	if (m_CurrentMahjongWishPower != 0 &&
		m_pTichuGame->CanFulfillWish(static_cast<uint8_t>(m_CurrentMahjongWishPower), playerWhoPassed->GetCards()))
	{
		//The player CAN fulfill the wish, so he's not allowed to pass
		return;
	}

	const std::pair<bool, bool> booleanInfo = m_pTichuGame->Pass();
	if (booleanInfo.first) //Did the player pass?
	{
		ody::ServiceLocator::GetSoundSystem().PlaySound(8);

		if (booleanInfo.second) // Did all the players pass
		{
			m_pAnnouncementText->SetVisible(false);

			const int points = m_pTichuGame->CountPoints(m_PlayedCards);
			if (m_CardsOnTop.size() == 1 && m_CardsOnTop[0].colour == CC_Dragon)
			{
				m_pPlayers[m_PlayerWhoThrewLastCombinationIndex]->AskForDragon();
			}
			else
			{
				m_pTichuGame->NextPlayer();
				UpdateLights();
				m_pPlayers[m_pTichuGame->GetCurrentPlayerIndex()]->GivePoints(points); //This is the current player because the index gets incremented inside Pass()

				m_CardsOnTop.clear();
				m_PlayedCards.clear();
				return;
			}
		}

		m_pTichuGame->NextPlayer();
		UpdateLights();
	}
}

void TichuScene::NewRound(bool isOneTwo)
{
	m_NumberOfPlayersOut = 0;
	m_CurrentMahjongWishPower = 0;

	int indexOfPlayerNotOut = 0;
	for (const auto& player : m_pPlayers)
	{
		//Handle tichu / grand tichu
		if (player->HasDeclaredTichu() || player->HasDeclaredGrandTichu())
		{
			//Get the ID of the player
			const int playerID = player->GetPlayerID();
			int pointGain = player->HasDeclaredTichu() ? 100 : 200;
			if (playerID != m_IndexOfFirstPlayerOut) //Compare it to the ID of the player who went out first
			{
				pointGain *= -1; //If they are not the same, the team loses the points
			}

			//Apply the points to the correct team
			if (playerID == 0 || playerID == 2) m_Team0Points += pointGain;
			else m_Team1Points += pointGain;
		}

		if (!player->IsOut())
		{
			indexOfPlayerNotOut = player->GetPlayerID();
		}
		
		player->SetOut(false);
		player->SetPlaying(false);
		player->RemoveDeclarations();
	}

	//Account for the 1-2 rule
	if (isOneTwo)
	{
		if (m_IndexOfFirstPlayerOut == 0 || m_IndexOfFirstPlayerOut == 2)
			m_Team0Points += 200;
		else
			m_Team1Points += 200;
	}
	else UpdatePlayerPoints(indexOfPlayerNotOut);

	if (m_Team0Points >= m_MaxPoints || m_Team1Points >= m_MaxPoints)
	{
		if (m_Team0Points >= m_MaxPoints)
			++m_Team0GamesWon;
		else
			++m_Team1GamesWon;

		GameOver();
		m_Team0Points = 0;
		m_Team1Points = 0;
	}

	m_Team0PointsText->SetText(std::to_string(m_Team0Points));
	m_Team1PointsText->SetText(std::to_string(m_Team1Points));

	m_PlayersAskedForGrandTichu = 0;
	m_PlayersWhoTradedCards = 0;
	m_GamePhase = GamePhase::GrandTichu;
	DealInitialCards(14); //todo: put this back to 8
	UpdateLights();

	m_CardsOnTop.clear();
	m_PlayedCards.clear();
}

void TichuScene::UpdateLights() const
{
	for (const auto& player : m_pPlayers)
	{
		player->SetPlaying(false);
	}

	if (m_GamePhase == GamePhase::GrandTichu)
	{
		m_pPlayers[m_PlayersAskedForGrandTichu]->SetPlaying(true);
	}
	else if (m_GamePhase == GamePhase::TradeCards)
	{
		m_pPlayers[m_PlayersWhoTradedCards]->SetPlaying(true);
	}
	else
		m_pPlayers[m_pTichuGame->GetCurrentPlayerIndex()]->SetPlaying(true);
}

void TichuScene::UpdatePlayerPoints(int indexOfPlayerNotOut)
{
	//The player who didn't go out has to give his points to the player who went out first
	m_pPlayers[m_IndexOfFirstPlayerOut]->GivePoints(m_pPlayers[indexOfPlayerNotOut]->GetPoints());
	m_pPlayers[indexOfPlayerNotOut]->ResetPoints();

	//Any points on the player's hand who didn't go out go to the opposite team
	const int pointsOnHand = m_pTichuGame->CountPoints(m_pPlayers[indexOfPlayerNotOut]->GetCards());
	const int oppositePlayerIndex = indexOfPlayerNotOut + 1 > 3 ? 0 : indexOfPlayerNotOut + 1;
	m_pPlayers[oppositePlayerIndex]->GivePoints(pointsOnHand);

	m_Team0Points += m_pPlayers[0]->GetPoints() + m_pPlayers[2]->GetPoints();
	m_Team1Points += m_pPlayers[1]->GetPoints() + m_pPlayers[3]->GetPoints();

	for (const auto& player : m_pPlayers)
		player->ResetPoints();
}

void TichuScene::DeclareTichu() const
{
	const auto player = m_pPlayers[m_pTichuGame->GetCurrentPlayerIndex()];
	player->DeclareTichu();
	//m_pTichuButton->SetVisible(false);
}

void TichuScene::DeclareGrandTichu()
{
	const auto player = m_pPlayers[m_PlayersAskedForGrandTichu];
	player->DeclareGrandTichu();
	DealRestOfCards();
	++m_PlayersAskedForGrandTichu;
	if (m_PlayersAskedForGrandTichu < 4)
		UpdateLights();
}

void TichuScene::DeclineGrandTichu()
{
	DealRestOfCards();
	++m_PlayersAskedForGrandTichu;
	if (m_PlayersAskedForGrandTichu < 4)
		UpdateLights();
}

void TichuScene::SetAnnouncementText(const std::string& text) const
{
	m_pAnnouncementText->SetText(text);
	m_pAnnouncementText->SetPosition(ody::constants::g_ScreenWidth / 2 - m_pAnnouncementText->GetTextSize().x / 2, 300);
	m_pAnnouncementText->SetVisible(true);
}

void TichuScene::HandleMahjongTable()
{
	bool showMahjongSelectionTable = false;

	for (const auto& player : m_pPlayers)
	{
		if (player->GetShowMahjongSelectionTable())
		{
			showMahjongSelectionTable = true;
			break;
		}
	}

	if (showMahjongSelectionTable && !m_IsMahjongSelectionTableVisible)
	{
		ShowMahjongSelectionTable(true);
	}
	else if (!showMahjongSelectionTable && m_IsMahjongSelectionTableVisible)
	{
		ShowMahjongSelectionTable(false);
		for (const auto& button : m_pMahjongButtons)
		{
			button->SetEnabled(true);
		}
		m_CurrentMahjongWishPower = 0;
	}
}

void TichuScene::GameOver() const
{
	m_GamesWonCounter->SetText(std::to_string(m_Team0GamesWon) + " - " + std::to_string(m_Team1GamesWon));
	ody::ServiceLocator::GetSoundSystem().PlaySound(10);
}

void TichuScene::ShowMahjongSelectionTable(const bool show)
{
	m_IsMahjongSelectionTableVisible = show;
	for (const auto textComponent : m_pMahjongButtonTextComponents)
	{
		textComponent->SetVisible(show);
	}

	for (const auto button : m_pMahjongButtons)
	{
		button->SetVisible(show);
	}
}

void TichuScene::ConfirmCardTrades()
{
	//Check that all cards have been selected
	for (const bool cardSelected : m_CardsSelectedForTrade)
	{
		if (!cardSelected)
		{
			SetAnnouncementText("You have to select 3 cards to trade!");
			return;
		}
	}

	//Reset the array
	std::fill(m_CardsSelectedForTrade.begin(), m_CardsSelectedForTrade.end(), false);

	++m_PlayersWhoTradedCards;

	for (const auto button : m_pTradeTableSelections)
	{
		std::shared_ptr<ody::Texture2D> texture = ody::ResourceManager::GetInstance().LoadTexture("Cards/back.png");
		button->SetTexture(texture);
	}

	if (m_PlayersWhoTradedCards >= 4) return;
	const auto cards = m_pPlayers[m_PlayersWhoTradedCards]->GetCards();
	for (int i{}; i < 14; ++i)
	{
		//Remove the effect the buttons had on them
		m_pTradeTableButtons[i]->SetEnabled(true);

		const size_t textureIndex = GetCardTextureIndex(cards[i]);
		m_pTradeTableButtons[i]->SetTexture(m_RenderPackage.cardTextures[textureIndex]);
	}

	UpdateLights();
}

void TichuScene::CreateMahjongSelectionTable()
{
	/* constexpr glm::vec2 buttonSize{ 37, 55 };
	constexpr float startingPosition = (ody::constants::g_ScreenWidth - (13 * buttonSize.x)) / 2.f;
	for (int i{ 0 }; i < 13; ++i)
	{
		auto createButtonCallback = [this](int buttonIndex) {
			return [this, buttonIndex]() {
				for (const auto& button : m_pMahjongButtons)
				{
					button->SetEnabled(true);
				}
				ody::ServiceLocator::GetSoundSystem().PlaySound(0);
				m_CurrentMahjongWishPower = buttonIndex + 2;
				m_pMahjongButtons[buttonIndex]->SetEnabled(false);
				};
			};

		m_pMahjongButtons.emplace_back(m_pButtonManager->AddButton(
			"Button.png",
			createButtonCallback(i),
			{ startingPosition + buttonSize.x * i, 600 },
			buttonSize
		));
	}

	for (int i{ 2 }; i < 11; ++i)
	{
		CreateButtonTextAtPosition(std::to_string(i), { startingPosition + buttonSize.x * (i - 2), 600 }, buttonSize);
	}

	CreateButtonTextAtPosition("J", { startingPosition + buttonSize.x * 9, 600 }, buttonSize);
	CreateButtonTextAtPosition("Q", { startingPosition + buttonSize.x * 10, 600 }, buttonSize);
	CreateButtonTextAtPosition("K", { startingPosition + buttonSize.x * 11, 600 }, buttonSize);
	CreateButtonTextAtPosition("A", { startingPosition + buttonSize.x * 12, 600 }, buttonSize);

	for (const auto textComponent : m_pMahjongButtonTextComponents)
	{
		textComponent->SetVisible(false);
	}

	for (const auto button : m_pMahjongButtons)
	{
		button->SetVisible(false);
	} */
}

void TichuScene::CreateButtonTextAtPosition(const std::string& text, const glm::vec2& position, const glm::vec2& buttonSize)
{
	const auto gameObject = CreateGameObject();
	const auto textComponent = gameObject->AddComponent<ody::TextComponent>(text, "bonzai.ttf", 40);
	const float textWidth = textComponent->GetTextSize().x;
	const float textOffset = (buttonSize.x - textWidth) / 2.f;
	textComponent->SetPosition(position.x + textOffset, position.y);

	m_pMahjongButtonTextComponents.emplace_back(textComponent);
}

void TichuScene::CreateTradeTable()
{
	/* m_CardsSelectedForTrade.resize(3);
	m_CardsSelectedForTradeIndex.resize(3);
	m_CardsForTrade.resize(12);

	glm::vec2 buttonSize{ 90, 120 };
	constexpr float gap = 10.f;
	float startingPosition = (ody::constants::g_ScreenWidth - (3 * buttonSize.x)) / 2.f;

	auto cardDestinationButtonCallback = [this](int buttonIndex)
		{
			// Capture buttonIndex by value to ensure it's the correct value when the lambda is executed
			return [this, buttonIndex]() -> void
				{
					m_CurrentSelectedTradingSlotIndex = buttonIndex;
					//Give it a light gray tint
					constexpr glm::vec4 selectedTint = { 0.65f, 0.65f, 0.65f, 1.f };
					constexpr glm::vec4 notSelectedTint = { 0.5f, 0.5f, 0.5f, 1.f };
					m_pTradeTableSelections[buttonIndex]->SetHoveredTint(selectedTint);
					m_pTradeTableSelections[buttonIndex]->SetNormalTint(selectedTint);

					//Reset the other buttons
					for (int i{}; i < 3; ++i)
					{
						if (i != buttonIndex)
						{
							m_pTradeTableSelections[i]->SetHoveredTint(notSelectedTint);
							m_pTradeTableSelections[i]->SetNormalTint({ 1.f, 1.f, 1.f, 1.f });
						}
					}

					if (m_CardsSelectedForTrade[buttonIndex])
					{
						m_CardsSelectedForTrade[buttonIndex] = false;
						m_pTradeTableSelections[buttonIndex]->SetTexture(ody::ResourceManager::GetInstance().LoadTexture("Cards/back.png"));
						m_CardsForTrade[m_PlayersWhoTradedCards * 3 + m_CurrentSelectedTradingSlotIndex] = Card{};
						m_pTradeTableButtons[m_CardsSelectedForTradeIndex[buttonIndex]]->SetEnabled(true);
					}
				};
		};

	for (int i{}; i < 3; ++i)
	{
		m_pTradeTableSelections.emplace_back(m_pButtonManager->AddButton("Cards/back.png", cardDestinationButtonCallback(i),
			{ startingPosition + (buttonSize.x + gap) * i, 400 }, buttonSize));
	}

	buttonSize = { 50, 80 };
	startingPosition = (ody::constants::g_ScreenWidth - (14 * buttonSize.x)) / 2.f;
	const auto cards = m_pPlayers[m_PlayersWhoTradedCards]->GetCards();
	for (int i{ 0 }; i < 14; ++i)
	{
		// Calculate the name of the card texture
		const size_t textureIndex = GetCardTextureIndex(cards[i]);

			auto cardSelectionButtonCallback = [this, i]()
				{
					return [this, i]()
						{
							if (m_CurrentSelectedTradingSlotIndex == -1) return;

							const auto playerCards = m_pPlayers[m_PlayersWhoTradedCards]->GetCards();
							// Calculate the name of the card texture
							const size_t buttonTextureIndex = GetCardTextureIndex(playerCards[i]);

							// Disable the current button
							m_pTradeTableButtons[i]->SetEnabled(false);
							ody::ServiceLocator::GetSoundSystem().PlaySound(0);

							// Load a new texture because the previous one has been Tinted from the button
							const auto texture = ody::ResourceManager::GetInstance().LoadTexture(m_RenderPackage.cardTexturePaths[buttonTextureIndex]);
							m_pTradeTableSelections[m_CurrentSelectedTradingSlotIndex]->SetTexture(texture);
							m_pTradeTableSelections[m_CurrentSelectedTradingSlotIndex]->SetNormalTint({ 1.f, 1.f, 1.f, 1.f });
							m_pTradeTableSelections[m_CurrentSelectedTradingSlotIndex]->SetHoveredTint({ 0.5f, 0.5f, 0.5f, 1.f });
							m_CardsSelectedForTrade[m_CurrentSelectedTradingSlotIndex] = true;
							m_CardsSelectedForTradeIndex[m_CurrentSelectedTradingSlotIndex] = i;

							// Access member variables directly to ensure current state
							m_CardsForTrade[m_PlayersWhoTradedCards * 3 + m_CurrentSelectedTradingSlotIndex] = m_pPlayers[m_PlayersWhoTradedCards]->GetCards()[i];

							m_CurrentSelectedTradingSlotIndex = -1;
						};
				};

			m_pTradeTableButtons.emplace_back(m_pButtonManager->AddButton(
				m_RenderPackage.cardTextures[textureIndex],
				cardSelectionButtonCallback(),
				{ startingPosition + buttonSize.x * i, 695 },
				buttonSize
			));
	} */
}

int TichuScene::GetCardTextureIndex(const Card& card) const
{
	switch (card.colour)
	{
	case CC_Dog:
		return 52;
	case CC_Dragon:
		return 53;
	case CC_Phoenix:
		return 54;
	case CC_Mahjong:
		return 55;
	default:
		return card.colour * 13 + card.power - 2;
	}
}

void TichuScene::FillGameState(MCTS::GameState &state) const
{
	//Fill in the player hands
    for (int i = 0; i < 4; ++i)
    {
        state.playerHands[i] = m_pPlayers[i]->GetCards();
    }

	//Fill in the team scores
	if (state.currentPlayerIndex == 1 || state.currentPlayerIndex == 3)
	{
		state.teamScore = static_cast<int16_t>(m_Team0Points);
		state.opponentTeamScore = static_cast<int16_t>(m_Team1Points);
	}
	else
	{
		state.teamScore = static_cast<int16_t>(m_Team1Points);
		state.opponentTeamScore = static_cast<int16_t>(m_Team0Points);
	}

	//Fill in the scores of the cards that each player has collected
	for (int i = 0; i < 4; ++i)
	{
		state.scores[i] = static_cast<int8_t>(m_pPlayers[i]->GetPoints());
	}

	state.passesInARow = static_cast<int8_t>(m_pTichuGame->GetPassesInARow());
	state.currentCombination = m_pTichuGame->GetCurrentStrongestCombination();
	state.lastPlayerIndex = m_PlayerWhoThrewLastCombinationIndex;
	state.pTichuGame = m_pTichuGame.get();
	state.cardsOnTable = m_PlayedCards;

	//todo: fill in the tichu and grand tichu booleans
}


void TichuScene::OnGUI()
{
	if (ImGui::CollapsingHeader("Debug"))
	{
		ImGui::Checkbox("Show card colliders", &m_ShowCardHitboxes);

		for (const auto& player : m_pPlayers)
		{
			player->ShowCardHitBoxes(m_ShowCardHitboxes);
		}

		ImGui::Checkbox("Show card backs", &m_ShowCardBacks);

		for (const auto& player : m_pPlayers)
		{
			player->ShowCardBacks(m_ShowCardBacks);
		}

		if (ImGui::Button("Deal Cards", { 90, 25 }))
		{
			DealInitialCards(13);
		}
	}

	if (ImGui::CollapsingHeader("Selected Combination", ImGuiTreeNodeFlags_DefaultOpen))
	{
		//Not very efficient but it's debug only
		auto cards = m_pPlayers[m_pTichuGame->GetCurrentPlayerIndex()]->GetHand();
		std::sort(cards.begin(), cards.end());

		const Combination combo = m_pTichuGame->CreateCombination(cards);

		// Display combination type
		const char* comboTypeStr = "Invalid";
		switch (combo.combinationType)
		{
		case CombinationType::CT_Single: comboTypeStr = "Single"; break;
		case CombinationType::CT_Doubles: comboTypeStr = "Doubles"; break;
		case CombinationType::CT_Triples: comboTypeStr = "Triples"; break;
		case CombinationType::CT_Straight: comboTypeStr = "Straight"; break;
		case CombinationType::CT_Steps: comboTypeStr = "Steps"; break;
		case CombinationType::CT_FullHouse: comboTypeStr = "Full House"; break;
		case CombinationType::CT_Invalid:comboTypeStr = "Invalid"; break;
		case CombinationType::CT_Dogs: comboTypeStr = "Dogs"; break;
		case CombinationType::CT_SinglePhoenix: comboTypeStr = "Single"; break;
		case CombinationType::CT_FourOfAKindBomb: comboTypeStr = "Four of a Kind Bomb"; break;
		case CombinationType::CT_StraightBomb: comboTypeStr = "Straight Bomb"; break;
		}
		ImGui::Text("Combination Type: %s", comboTypeStr);

		// Display number of cards
		ImGui::Text("Number of Cards: %d", combo.numberOfCards);

		// Display power
		ImGui::Text("Power: %d", combo.power);

		// Display validity
		const bool isValid = (combo.combinationType != CombinationType::CT_Invalid);
		ImGui::TextColored(isValid ? ImVec4(0, 1, 0, 1) : ImVec4(1, 0, 0, 1),
			isValid ? "Valid Combination" : "Invalid Combination");
	}
}