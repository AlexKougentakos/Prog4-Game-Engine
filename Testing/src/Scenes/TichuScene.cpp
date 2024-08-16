#include "TichuScene.h"

#include <imgui.h>
#include <random>

#include "GameObject.h"
#include "TextureComponent.h"
#include "InputManager2.h"
#include "ImGuiManager.h"
#include "Commands/CardSelectCommand.h"

void TichuScene::Initialize()
{
	const auto gameObject = CreateGameObject();
	gameObject->AddComponent<ody::TextureComponent>("cloth.png");
	
	CreateCardRenderPackage();
	CreateDeck();
	CreatePlayers();
	DealCards();

	m_pTichuGame = std::make_unique<Tichu>();

	ody::InputManager::GetInstance().AddMouseCommand(SDL_BUTTON_LEFT, ody::InputManager::InputType::OnMouseButtonDown, std::make_unique<CardSelectCommand>(m_pPlayers, m_CurrentPlayerIndex));
}

void TichuScene::PostRender() 
{
	const float cardHeight = static_cast<float>(renderPackage.cardTextures[0]->GetSize().y);
	const float cardWidth = static_cast<float>(renderPackage.cardTextures[0]->GetSize().x);
	const float stackWidth = renderPackage.cardSpacing * (m_CurrentCards.size() - 1) + cardWidth * renderPackage.cardScale;
	const auto startPosition = glm::vec3(ody::constants::g_ScreenWidth / 2.f - stackWidth / 2, ody::constants::g_ScreenHeight / 2.f - (cardHeight * renderPackage.cardScale) / 2.f, 0);

	for (size_t i = 0; i < m_CurrentCards.size(); ++i)
	{
		//Move them to the side slightly, so you can see all the cards
		const glm::vec3 cardPosition = startPosition + glm::vec3{ renderPackage.cardSpacing, 0, 0 } *static_cast<float>(i);

		// Calculate the name of the card texture
		const size_t textureIndex = m_CurrentCards[i].colour * 13 + m_CurrentCards[i].power - 2;

		// Render the texture with rotation
		ody::Renderer::GetInstance().RenderTexture(
			*renderPackage.cardTextures[textureIndex],
			cardPosition.x,
			cardPosition.y,
			cardWidth,
			cardHeight,
			1.5
		);
	}
}


void TichuScene::Update()
{

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

		const auto playerComponent = player->AddComponent<PlayerComponent>(i, renderPackage);
		m_pPlayers.emplace_back(playerComponent);
	}
}


void TichuScene::CreateCardRenderPackage()
{
	renderPackage.cardTextures.clear();
	renderPackage.cardScale = 1.5f;
	renderPackage.cardSpacing = 25.f;

	for (int i = 0; i < 52; ++i) //todo: make this 56 cards later
	{
		std::string cardName = std::to_string(i);
		std::string paddedCardName = std::string(3 - cardName.length(), '0') + cardName;
		std::string completePath = "Cards/tile" + paddedCardName + ".png";

		auto texture = ody::ResourceManager::GetInstance().LoadTexture(completePath);
		renderPackage.cardTextures.emplace_back(texture);
	}
}

void TichuScene::DealCards()
{
	//Deletes the old cards and creates new ones
	//You can just shuffle the current ones but it's done once per round only
	CreateDeck();

	for (const auto& player : m_pPlayers)
	{
		constexpr int numberOfCards = 13;
		// Calculate the start and end indices for this player's cards
		const int startIndex = player->GetPlayerID() * numberOfCards;
		const int endIndex = startIndex + numberOfCards;

		// Create a vector of cards for this player
		std::vector<Card> playerCards(m_Cards.begin() + startIndex, m_Cards.begin() + endIndex);
		player->SetCards(playerCards);
	}
}

void TichuScene::CheckSubmittedHand()
{
	std::vector<Card> submittedHand = m_pPlayers[m_CurrentPlayerIndex]->GetHand();
	std::sort(submittedHand.begin(), submittedHand.end());

	const Combination combination = Tichu::CreateCombination(submittedHand);

	if (m_pTichuGame->PlayHand(combination))
	{
		m_pPlayers[m_CurrentPlayerIndex]->PlayedSelectedCards();
		NextPlayer();
		m_CurrentCards = submittedHand;
	}
}

void TichuScene::NextPlayer()
{
	do
	{
		// Increment the current player index
		m_CurrentPlayerIndex = (m_CurrentPlayerIndex + 1) % 4;
	} while (m_pPlayers[m_CurrentPlayerIndex]->IsOut());
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

		if (ImGui::Button("Deal Cards", { 90, 25 }))
		{
			DealCards();
		}
	}

	if (ImGui::CollapsingHeader("Game Moves"), ImGuiTreeNodeFlags_DefaultOpen)
	{
		ImGui::SliderInt("Current Player", &m_CurrentPlayerIndex, 0, 3);

		if (ImGui::Button("Play Cards", { 90, 25 }))
		{
			CheckSubmittedHand();
		}

		ImGui::SameLine();
		if (ImGui::Button("Pass", { 90, 25 }))
		{
			//
		}
	}

	if (ImGui::CollapsingHeader("Selected Combination"), ImGuiTreeNodeFlags_DefaultOpen)
	{
		const Combination combo = Tichu::CreateCombination(m_pPlayers[m_CurrentPlayerIndex]->GetHand());

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
