#include "TichuScene.h"
#include "GameObject.h"
#include "TextureComponent.h"
#include "InputManager2.h"
#include "Components/CardComponent.h"
#include "ImGuiManager.h"

void TichuScene::Initialize()
{
	const auto gameObject = CreateGameObject();
	gameObject->AddComponent<ody::TextureComponent>("background.tga");
	
	for (int i = 0; i < 52; i++)
	{
		const auto card = CreateGameObject();
		card->AddComponent<ody::TextureComponent>("Cards/tile" + std::string(3 - std::to_string(i).length(), '0') + std::to_string(i) + ".png");

		m_pCards.emplace_back(card);

		//Move each card to the right and then down every 12 cars
		const uint8_t cardValue = static_cast<uint8_t>(i % 13 + 2); //This is because 'i' starts at 0 and the cards start at 2
		CardColour colour{static_cast<uint8_t>(i/13)};
		card->AddComponent<CardComponent>(colour, cardValue);
		card->GetTransform()->SetPosition(50.0f + (i % 13) * 50.0f, 50.0f + (i / 13) * 50.0f);
	}
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