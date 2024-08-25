#include "TichuScene.h"

#include <imgui.h>
#include <random>

#include "GameObject.h"
#include "TextureComponent.h"
#include "InputManager2.h"
#include "ImGuiManager.h"
#include "Commands/ButtonPressed.h"
#include "Commands/CardSelectCommand.h"
#include "ButtonManagerComponent.h"
#include "ServiceLocator.h"
#include "TextComponent.h"

void TichuScene::Initialize()
{
	const auto pLevel = CreateGameObject();
	pLevel->AddComponent<ody::TextureComponent>("cloth.png");
	m_pButtonManager = pLevel->AddComponent<ButtonManagerComponent>();

	CreatePointDisplay();

	m_pTichuGame = std::make_unique<Tichu>();
	
	CreateCardRenderPackage();
	CreateDeck();
	CreatePlayers();

	DealInitialCards();

	ody::InputManager::GetInstance().AddMouseCommand<CardSelectCommand>(SDL_BUTTON_LEFT, ody::InputManager::InputType::OnMouseButtonDown, m_pPlayers, m_pTichuGame->GetCurrentPlayerIndex());
	ody::InputManager::GetInstance().AddMouseCommand<ButtonPressed>(SDL_BUTTON_LEFT, ody::InputManager::InputType::OnMouseButtonDown, m_pButtonManager);

	m_pPassButton = m_pButtonManager->AddButton("PassButton.png", [&]() 
		{ 
		Pass();
		ody::ServiceLocator::GetSoundSystem().PlaySound(8);
		} , { 400, 660 });
	m_pPassButton->SetEnabled(false); //We start on an empty table so you can't say pass

	m_pPlayButton = m_pButtonManager->AddButton("PlayButton.png", [&]() 
		{
			CheckSubmittedHand();
			ody::ServiceLocator::GetSoundSystem().PlaySound(0);
		} , { 530, 660 });
	m_pPlayButton->SetEnabled(false); //We will first ask everyone for grand tichu, then you can play

	m_pTichuButton = m_pButtonManager->AddButton("TichuButton.png", [&]() 
		{
			DeclareTichu(); 
			ody::ServiceLocator::GetSoundSystem().PlaySound(11);
		}, { 185, 660 });
	m_pTichuButton->SetVisible(false);

	m_pGrandTichuButton = m_pButtonManager->AddButton("GrandTichuButton.png", [&]() 
		{
			DeclareGrandTichu(); 
			ody::ServiceLocator::GetSoundSystem().PlaySound(11);
		}, { 185, 660 });
	m_pDealCardsButton = m_pButtonManager->AddButton("DealButton.png", [&]() 
		{ 
			DeclineGrandTichu();
			ody::ServiceLocator::GetSoundSystem().PlaySound(0);
		}, { 45, 660 });

	const auto scoreCounter = CreateGameObject();
	m_GamesWonCounter = scoreCounter->AddComponent<ody::TextComponent>("0 - 0", "bonzai.ttf", 40);
	m_GamesWonCounter->SetPosition(static_cast<float>(ody::constants::g_ScreenWidth / 2) - scoreCounter->GetComponent<ody::TextComponent>()->GetTextSize().x / 2, -5);

	CreateMahjongSelectionTable();

	constexpr int screenWidth = ody::constants::g_ScreenWidth;
	constexpr int screenHeight = ody::constants::g_ScreenHeight;
	constexpr int offsetFromEdge = 190;
	constexpr int buttonSize = 40;

	auto button = m_pButtonManager->AddButton("Button.png", [&]() {GiveDragonToPlayer(0); }, { screenWidth / 2 - buttonSize / 2, screenHeight - offsetFromEdge - buttonSize }, { buttonSize, buttonSize });
	button->SetVisible(false);
	m_pDragonButtons.emplace_back(button);
	button = m_pButtonManager->AddButton("Button.png", [&]() {GiveDragonToPlayer(1); }, { offsetFromEdge, screenHeight / 2 - buttonSize / 2 + m_RenderPackage.pointDisplayHeight / 2 }, { buttonSize, buttonSize });
	button->SetVisible(false);
	m_pDragonButtons.emplace_back(button);
	button = m_pButtonManager->AddButton("Button.png", [&]() {GiveDragonToPlayer(2); }, { screenWidth / 2 - buttonSize / 2, offsetFromEdge + m_RenderPackage.pointDisplayHeight }, { buttonSize, buttonSize });
	button->SetVisible(false);
	m_pDragonButtons.emplace_back(button);
	button = m_pButtonManager->AddButton("Button.png", [&]() {GiveDragonToPlayer(3); }, { screenWidth - offsetFromEdge - buttonSize, screenHeight / 2 - buttonSize / 2 + m_RenderPackage.pointDisplayHeight / 2 }, { buttonSize, buttonSize });
	button->SetVisible(false);
	m_pDragonButtons.emplace_back(button);

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

	if (m_GamePhase == GamePhase::GrandTichu && m_PlayersAskedForGrandTichu >= 4)
	{
		m_GamePhase = GamePhase::Playing;
		UpdateTichuButton();
		UpdateLights();
		m_pGrandTichuButton->SetVisible(false);
		m_pDealCardsButton->SetVisible(false);
		m_pPlayButton->SetEnabled(true);
	}

	//todo: add a dirty flag for this
	std::vector<PlayerState> playerStates{};

	for (const auto& player : m_pPlayers)
	{
		PlayerState playerState{};
		playerState.isOut = player->IsOut();
		playerStates.emplace_back(playerState);
	}

	m_pTichuGame->UpdatePlayerStates(playerStates);
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
	//Emscripten doesn't have support for ranges yet
#ifndef __EMSCRIPTEN__
	std::ranges::shuffle(m_Cards, g);
#else
	std::shuffle(m_Cards.begin(), m_Cards.end(), g);
#endif
}

void TichuScene::CreatePlayers()
{
	for (int i = 0; i < 4; ++i)
	{
		const auto player = CreateGameObject();

		const auto playerComponent = player->AddComponent<PlayerComponent>(i, m_RenderPackage);
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
	}

	//Dogs
	std::string completePath = "Cards/Dogs.png";
	auto texture = ody::ResourceManager::GetInstance().LoadTexture(completePath);
	m_RenderPackage.cardTextures.emplace_back(texture);

	//Dragon
	completePath = "Cards/Dragon.png";
	texture = ody::ResourceManager::GetInstance().LoadTexture(completePath);
	m_RenderPackage.cardTextures.emplace_back(texture);

	//Phoenix
	completePath = "Cards/Phoenix.png";
	texture = ody::ResourceManager::GetInstance().LoadTexture(completePath);
	m_RenderPackage.cardTextures.emplace_back(texture);

	//Mahjong
	completePath = "Cards/Mahjong.png";
	texture = ody::ResourceManager::GetInstance().LoadTexture(completePath);
	m_RenderPackage.cardTextures.emplace_back(texture);


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

void TichuScene::DealInitialCards()
{
	//Deletes the old cards and creates new ones
	//You can just shuffle the current ones but it's done once per round only
	CreateDeck();

	for (const auto& player : m_pPlayers)
	{
		constexpr int numberOfCards = 8;
		// Calculate the start and end indices for this player's cards
		const int startIndex = player->GetPlayerID() * numberOfCards;
		const int endIndex = startIndex + numberOfCards;

		// Create a vector of cards for this player
		std::vector<Card> playerCards(m_Cards.begin() + startIndex, m_Cards.begin() + endIndex);
			
		std::sort(playerCards.begin(), playerCards.end());
		player->SetCards(playerCards);
	}
}

void TichuScene::DealRestOfCards()
{
	if (m_PlayersAskedForGrandTichu >= m_pPlayers.size())
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

	// Check for Mahjong card
	if (std::find(playerCards.begin(), playerCards.end(), Card{ CardColour::CC_Mahjong, 1 }) != playerCards.end())
	{
		player->SetPlaying(true);
		m_pTichuGame->SetStartingPlayer(player->GetPlayerID());
	}

	// Sort the entire hand
	std::sort(playerCards.begin(), playerCards.end());
	player->SetCards(playerCards);
}

void TichuScene::CheckSubmittedHand()
{
	const auto player = m_pPlayers[m_pTichuGame->GetCurrentPlayerIndex()];

	std::vector<Card> submittedHand = player->GetHand();
	std::sort(submittedHand.begin(), submittedHand.end());

	//todo: Check HERE to see if the player needs to follow the mahjong wish
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
			AddAnnouncementText("You have to fulfill the wish of the card: " + std::to_string(m_CurrentMahjongWishPower));
			return;
		}
		else 
		{
			m_CurrentMahjongWishPower = 0;
		}
	}

	const Combination combination = m_pTichuGame->CreateCombination(submittedHand);
	if (combination.combinationType == CombinationType::CT_Dogs)
		m_PlayerWhoThrewDogsIndex = m_pTichuGame->GetCurrentPlayerIndex();

	if (m_pTichuGame->PlayHand(combination))
	{
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
		UpdateTichuButton();

		m_pPassButton->SetEnabled(true);
		const int previousPlayerIndex = combination.combinationType == CombinationType::CT_Dogs ? m_PlayerWhoThrewDogsIndex : m_pTichuGame->GetPreviousPlayerIndex();

		AddAnnouncementText("Player " + std::to_string(previousPlayerIndex) + " played a hand!");
		
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
				if (m_pPlayers[0]->IsOut() && m_pPlayers[2]->IsOut() ||
					m_pPlayers[1]->IsOut() && m_pPlayers[3]->IsOut())
				NewRound(true);

				return;
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

void TichuScene::GiveDragonToPlayer(const int playerID) const
{
	m_pPlayers[playerID]->GivePoints(25);
	m_pPlayButton->SetEnabled(true);

	for (const auto& button : m_pDragonButtons)
	{
		button->SetVisible(false);
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

				const int teamWhoGotTheHand = m_pTichuGame->GetCurrentPlayerIndex() == 0 || m_pTichuGame->GetCurrentPlayerIndex() == 2 ? 0 : 1;
				if (teamWhoGotTheHand == 0)
				{
					m_pDragonButtons[1]->SetVisible(true);
					m_pDragonButtons[3]->SetVisible(true);
				}
				else
				{
					m_pDragonButtons[0]->SetVisible(true);
					m_pDragonButtons[2]->SetVisible(true);
				}
				m_pPassButton->SetEnabled(false);
				m_pPlayButton->SetEnabled(false);
			}
			else
				m_pPlayers[m_pTichuGame->GetCurrentPlayerIndex()]->GivePoints(points); //This is the current player because the index gets incremented inside Pass()

			m_CardsOnTop.clear();
			m_PlayedCards.clear();
			m_pPassButton->SetEnabled(false);
		}

		playerWhoPassed->Pass();
		UpdateTichuButton();
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
	m_GamePhase = GamePhase::GrandTichu;
	m_pGrandTichuButton->SetVisible(true);
	m_pDealCardsButton->SetVisible(true);
	m_pPlayButton->SetEnabled(false);
	m_pPassButton->SetEnabled(false);
	m_pTichuButton->SetVisible(false);

	DealInitialCards();
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
	m_pTichuButton->SetVisible(false);
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

void TichuScene::AddAnnouncementText(const std::string& text) const
{
	m_pAnnouncementText->SetText(text);
	m_pAnnouncementText->SetPosition(ody::constants::g_ScreenWidth / 2 - m_pAnnouncementText->GetTextSize().x / 2, 300);
	m_pAnnouncementText->SetVisible(true);
}

void TichuScene::UpdateTichuButton() const
{
	const auto player = m_pPlayers[m_pTichuGame->GetCurrentPlayerIndex()];
	//You can only call tichu when you have 14 cards
	if (player->GetCards().size() == 14 && !player->HasDeclaredGrandTichu())
	{
		m_pTichuButton->SetVisible(true);
	}
	else
	{
		m_pTichuButton->SetVisible(false);
	}
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

void TichuScene::GameOver()
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

	for (auto button : m_pMahjongButtons)
	{
		button->SetVisible(show);
	}
}

void TichuScene::CreateMahjongSelectionTable()
{
	constexpr glm::vec2 buttonSize{ 37, 55 };
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
	}
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
			DealInitialCards();
		}
	}

	if (ImGui::CollapsingHeader("Game Moves"), ImGuiTreeNodeFlags_DefaultOpen)
	{
		if (ImGui::Button("Play Cards", { 90, 25 }))
		{
			CheckSubmittedHand();
		}

		ImGui::SameLine();
		if (ImGui::Button("Pass", { 90, 25 }))
		{
			Pass();
		}
	}

	if (ImGui::CollapsingHeader("Selected Combination"), ImGuiTreeNodeFlags_DefaultOpen)
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