#include "TichuScene.h"

#include <random>

#include "GameObject.h"
#include "TextureComponent.h"
#include "InputManager2.h"
#include "ImGuiManager.h"

void TichuScene::Initialize()
{
	const auto gameObject = CreateGameObject();
	gameObject->AddComponent<ody::TextureComponent>("background.tga");
	
	for (int i = 0; i < 52; i++)
	{
		const uint8_t cardValue = static_cast<uint8_t>(i % 13 + 2); //This is because 'i' starts at 0 and the cards start at 2
		const CardColour colour{static_cast<uint8_t>(i/13)};
		m_Cards.emplace_back(Card{colour, cardValue});
	}

	// Shuffle the deck before distributing
	std::random_device rd;
	std::mt19937 g(rd());
	
	std::ranges::shuffle(m_Cards, g);

	constexpr int numberOfCards = 13;
	for (int i = 0; i < 4; ++i)
	{
		const auto player = CreateGameObject();
    
		// Calculate the start and end indices for this player's cards
		const int startIndex = i * numberOfCards;
		const int endIndex = startIndex + numberOfCards;
    
		// Create a vector of cards for this player
		std::vector<Card> playerCards(m_Cards.begin() + startIndex, m_Cards.begin() + endIndex);
    
		const auto playerComponent = player->AddComponent<PlayerComponent>(i, playerCards);
		m_pPlayers.emplace_back(playerComponent);
	}
	//black green blue red
	
	// for (const auto player : m_pPlayers)
	// {
	// 	const auto playerCards = player->GetCards();
	// 	for (size_t i{}; i < playerCards.size(); ++i)
	// 	{
	// 		const auto card = CreateGameObject();
	// 		std::string cardName = std::to_string(playerCards[i].colour * 13 + playerCards[i].power - 2);
	// 		std::string paddedCardName = std::string(3 - cardName.length(), '0') + cardName;
	// 		std::string completePath = "Cards/tile" + paddedCardName + ".png";
	// 		card->AddComponent<ody::TextureComponent>("Cards/tile" + paddedCardName + ".png");
	// 		//Move each card to the right and then down every 12 cards
	// 		card->GetTransform()->SetPosition(50.0f + static_cast<float>(i % 13) * 50.0f, 50.0f + player->GetPlayerID() * 50.0f);
	// 	}
	// }
}

void TichuScene::Render()
{

}

void TichuScene::Update()
{

}

void TichuScene::OnGUI()
{

}