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
#include "TextComponent.h"

void TichuScene::Initialize()
{
	const auto pBackGround = CreateGameObject();
	pBackGround->AddComponent<ody::TextureComponent>("cloth.png");
	m_pButtonManager = pBackGround->AddComponent<ButtonManagerComponent>();

	const auto pPointDisplay = CreateGameObject();
	pPointDisplay->AddComponent<ody::TextureComponent>("PointDisplay.png");

	m_RenderPackage.pointDisplayHeight = pPointDisplay->GetComponent<ody::TextureComponent>()->GetTextureSize().y;

	m_pTichuGame = std::make_unique<Tichu>();
	
	CreateCardRenderPackage();
	CreateDeck();
	CreatePlayers();
	DealCards();

	ody::InputManager::GetInstance().AddMouseCommand<CardSelectCommand>(SDL_BUTTON_LEFT, ody::InputManager::InputType::OnMouseButtonDown, m_pPlayers, m_pTichuGame->GetCurrentPlayerIndex());
	ody::InputManager::GetInstance().AddMouseCommand<ButtonPressed>(SDL_BUTTON_LEFT, ody::InputManager::InputType::OnMouseButtonDown, m_pButtonManager);

	m_pPassButton = m_pButtonManager->AddButton("PassButton.png", [&]() { Pass(); } , { 185, 660 });
	m_pPassButton->SetEnabled(false); //We start on an empty table so you can't say pass
	m_pButtonManager->AddButton("PlayButton.png", [&]() { CheckSubmittedHand(); } , { 530, 660 });

	CreateMahjongSelectionTable();

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
	const float stackWidth = m_RenderPackage.cardSpacing * (m_CurrentCards.size() - 1) + cardWidth * m_RenderPackage.cardScale;
	const auto startPosition = glm::vec3(ody::constants::g_ScreenWidth / 2.f - stackWidth / 2, ody::constants::g_ScreenHeight / 2.f - (cardHeight * m_RenderPackage.cardScale) / 2.f, 0);

	for (size_t i = 0; i < m_CurrentCards.size(); ++i)
	{
		//Move them to the side slightly, so you can see all the cards
		const glm::vec3 cardPosition = startPosition + glm::vec3{ m_RenderPackage.cardSpacing, 0, 0 } *static_cast<float>(i);

		// Calculate the name of the card texture
		const size_t textureIndex = [&]() -> size_t
			{
				//This is the order in which they are added inside the CreateDeck() function in the TichuScene
				switch (m_CurrentCards[i].colour)
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
					return m_CurrentCards[i].colour * 13 + m_CurrentCards[i].power - 2; //Calculate the texture index using the colour to find the right tile image
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

void TichuScene::DealCards()
{
	//Deletes the old cards and creates new ones
	//You can just shuffle the current ones but it's done once per round only
	CreateDeck();

	for (const auto& player : m_pPlayers)
	{
		constexpr int numberOfCards = 14;
		// Calculate the start and end indices for this player's cards
		const int startIndex = player->GetPlayerID() * numberOfCards;
		const int endIndex = startIndex + numberOfCards;

		// Create a vector of cards for this player
		std::vector<Card> playerCards(m_Cards.begin() + startIndex, m_Cards.begin() + endIndex);
		if (std::find(playerCards.begin(), playerCards.end(), Card{ CardColour::CC_Mahjong, 1 }) != playerCards.end())
		{
			player->SetPlaying(true);
			m_pTichuGame->SetStartingPlayer(player->GetPlayerID());
		}
			
		std::sort(playerCards.begin(), playerCards.end());
		player->SetCards(playerCards);
	}
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
			return;
		}
		else 
		{
			m_CurrentMahjongWishPower = 0;
		}
	}

	const Combination combination = m_pTichuGame->CreateCombination(submittedHand);

	if (m_pTichuGame->PlayHand(combination))
	{
		m_pPassButton->SetEnabled(true);
		int previousPlayer = combination.combinationType == CombinationType::CT_Dogs ? m_pTichuGame->GetCurrentPlayerIndex() - 2 : m_pTichuGame->GetCurrentPlayerIndex() - 1;
		if (previousPlayer < 0) previousPlayer = 3;

		//Previous player because the current player index gets incremented inside PlayHand()

		m_pPlayers[previousPlayer]->PlayedSelectedCards();
		m_CurrentCards = submittedHand;
		UpdateLights();
		ShowMahjongSelectionTable(false);
	}
}

void TichuScene::Pass()
{
	const auto player = m_pPlayers[m_pTichuGame->GetCurrentPlayerIndex()];
	if (m_CurrentMahjongWishPower != 0 &&
		m_pTichuGame->CanFulfillWish(static_cast<uint8_t>(m_CurrentMahjongWishPower), player->GetCards()))
	{
		//The player CAN fulfill the wish, so he's not allowed to pass
		return;
	}

	const std::pair<bool, bool> booleanInfo = m_pTichuGame->Pass();
	if (booleanInfo.first)
	{
		if (booleanInfo.second)
		{
			m_CurrentCards.clear();
			m_pPassButton->SetEnabled(false);
		}

		player->Pass();
		UpdateLights();
	}

}

void TichuScene::UpdateLights() const
{
	for (const auto& player : m_pPlayers)
	{
		player->SetPlaying(false);
	}

	m_pPlayers[m_pTichuGame->GetCurrentPlayerIndex()]->SetPlaying(true);
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


void TichuScene::ShowMahjongSelectionTable(const bool show)
{
	m_IsMahjongSelectionTableVisible = show;
	for (const auto textComponent : m_pTextComponents)
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

	for (const auto textComponent : m_pTextComponents)
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

	m_pTextComponents.emplace_back(textComponent);
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
			DealCards();
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
