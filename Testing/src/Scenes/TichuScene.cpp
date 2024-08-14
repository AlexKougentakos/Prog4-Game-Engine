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
	gameObject->AddComponent<ody::TextureComponent>("cloth.jpg");
	
	CreateDeck();
	CreatePlayersWithCards();

	ody::InputManager::GetInstance().AddMouseCommand(SDL_BUTTON_LEFT, ody::InputManager::InputType::OnMouseButtonDown, std::make_unique<CardSelectCommand>(m_pPlayers[0]));
}

void TichuScene::Render()
{

}

void TichuScene::Update()
{

}

void TichuScene::CreateDeck()
{
	for (int i = 0; i < 52; i++)
	{
		const uint8_t cardValue = static_cast<uint8_t>(i % 13 + 2); //This is because 'i' starts at 0 and the cards start at 2
		const CardColour colour{ static_cast<uint8_t>(i / 13) };
		m_Cards.emplace_back(Card{ colour, cardValue });
	}

	// Shuffle the deck before distributing
	std::random_device rd;
	std::mt19937 g(rd());

	std::ranges::shuffle(m_Cards, g);
}

void TichuScene::CreatePlayersWithCards()
{
	for (int i = 0; i < 4; ++i)
	{
		constexpr int numberOfCards = 13;
		const auto player = CreateGameObject();

		// Calculate the start and end indices for this player's cards
		const int startIndex = i * numberOfCards;
		const int endIndex = startIndex + numberOfCards;

		// Create a vector of cards for this player
		std::vector<Card> playerCards(m_Cards.begin() + startIndex, m_Cards.begin() + endIndex);

		const auto playerComponent = player->AddComponent<PlayerComponent>(i, playerCards);
		m_pPlayers.emplace_back(playerComponent);
	}
}

void TichuScene::OnGUI()
{
	
	ImGui::Checkbox("Show card colliders", &m_ShowCardHitboxes);

	for (const auto& player : m_pPlayers)
	{
		player->ShowCardHitBoxes(m_ShowCardHitboxes);
	}

}
