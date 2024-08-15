#include "TichuScene.h"

#include <imgui.h>
#include <random>

#include "GameObject.h"
#include "TextureComponent.h"
#include "InputManager2.h"
#include "ImGuiManager.h"
#include "Commands/CardSelectCommand.h"
#include "Tichu.h"

void TichuScene::Initialize()
{
	const auto gameObject = CreateGameObject();
	gameObject->AddComponent<ody::TextureComponent>("cloth.png");
	
	CreateDeck();
	CreatePlayers();
	DealCards();

	ody::InputManager::GetInstance().AddMouseCommand(SDL_BUTTON_LEFT, ody::InputManager::InputType::OnMouseButtonDown, std::make_unique<CardSelectCommand>(m_pPlayers[1]));
}

void TichuScene::Render()
{

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

		const auto playerComponent = player->AddComponent<PlayerComponent>(i);
		m_pPlayers.emplace_back(playerComponent);
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

	Tichu::CreateCombination(submittedHand);


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

	if (ImGui::CollapsingHeader("Game Moves"))
	{
		ImGui::SliderInt("Current Player", &m_CurrentPlayerIndex, 0, 3);

		if (ImGui::Button("Play Cards", { 90, 25 }))
		{
			CheckSubmittedHand();
		}
	}

	if (ImGui::CollapsingHeader("Selected Combination"))
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
