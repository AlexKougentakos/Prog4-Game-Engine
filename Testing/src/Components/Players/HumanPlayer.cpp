#include "HumanPlayer.h"
#include "ButtonManagerComponent.h"
#include "ServiceLocator.h"
#include "GameObject.h"
#include "../ButtonManagerComponent.h"
#include <Commands/CardSelectCommand.h>
#include "../Commands/ButtonPressed.h"
#include "InputManager2.h"
#include "Tichu.h"

#include "glm/glm.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include <Structs.h>

HumanPlayer::HumanPlayer(const int playerID, const CardRenderPackage& renderPackage)
    : PlayerComponent(playerID, renderPackage)
{

}

void HumanPlayer::Initialize()
{
	PlayerComponent::Initialize();

	m_pButtonManager = GetOwner()->AddComponent<ButtonManagerComponent>();
	CreateActionButtons();

	ody::InputManager::GetInstance().AddMouseCommand<ButtonPressed>(SDL_BUTTON_LEFT, ody::InputManager::InputType::OnMouseButtonDown, m_pButtonManager);
	ody::InputManager::GetInstance().AddMouseCommand<CardSelectCommand>(SDL_BUTTON_LEFT, ody::InputManager::InputType::OnMouseButtonDown, this);
	
	m_pTichuButton->SetPosition({ m_pTichuButton->GetPosition().x, m_pTichuButton->GetPosition().y + 60 });
	m_pPlayButton->SetPosition({ m_pPlayButton->GetPosition().x, m_pPlayButton->GetPosition().y + 60 });
	m_pPassButton->SetPosition({ m_pPassButton->GetPosition().x, m_pPassButton->GetPosition().y + 60 });
	
	m_pPlayButton->SetEnabled(true);

	CalculateHitBoxes();
}

void HumanPlayer::Update()
{
    PlayerComponent::Update();
    CalculateHitBoxes();

    // Update button states based on if it's this player's turn
    if (m_IsPlaying)
    {
        m_pPlayButton->SetVisible(true);
        m_pPassButton->SetVisible(true);

        // Can't pass if table is empty and it's your turn
        const bool canPass = 
        m_pTichuGame->GetCurrentStrongestCombination().combinationType != CombinationType::CT_Invalid;
        m_pPassButton->SetEnabled(canPass);

        // Play button is enabled if cards are selected
        m_pPlayButton->SetEnabled(!m_SelectedCards.empty());
    }
    else
    {
        m_pPlayButton->SetVisible(false);
        m_pPassButton->SetVisible(false);
    }
}

void HumanPlayer::CreateActionButtons()
{
	m_pPassButton = m_pButtonManager->AddButton("PassButton.png", [&]()
		{
			Pass();
			ody::ServiceLocator::GetSoundSystem().PlaySound(8);
		}, { 507, 570 });
	m_pPassButton->SetEnabled(false); //We start on an empty table so you can't say pass

	m_pPlayButton = m_pButtonManager->AddButton("PlayButton.png", [&]()
		{
			PlayedSelectedCards();
			ody::ServiceLocator::GetSoundSystem().PlaySound(0);
		}, { 637, 570 });
	m_pPlayButton->SetEnabled(false); //We will first ask everyone for grand tichu, then you can play

	m_pTichuButton = m_pButtonManager->AddButton("TichuButton.png", [&]()
		{
			DeclareTichu();
			ody::ServiceLocator::GetSoundSystem().PlaySound(11);
		}, { 293, 570 });
	m_pTichuButton->SetVisible(false);

    constexpr int screenWidth = ody::constants::g_ScreenWidth;
	constexpr int screenHeight = ody::constants::g_ScreenHeight;
	constexpr int offsetFromEdge = 190;
	constexpr int buttonSize = 40;

	m_pGiveDragonButtonLeft = m_pButtonManager->AddButton("Button.png", [&]()
		{
			GiveDragonToPlayer(1);
		}, { offsetFromEdge, screenHeight / 2 - buttonSize / 2 + m_RenderPackage.pointDisplayHeight / 2  }, 
        { buttonSize, buttonSize });
	m_pGiveDragonButtonLeft->SetVisible(false);

	m_pGiveDragonButtonRight = m_pButtonManager->AddButton("Button.png", [&]()
		{
			GiveDragonToPlayer(3);
		}, { screenWidth - offsetFromEdge - buttonSize, screenHeight / 2 - buttonSize / 2 + m_RenderPackage.pointDisplayHeight / 2 }, 
        { buttonSize, buttonSize });
	m_pGiveDragonButtonRight->SetVisible(false);

/*
* REMOVED FOR THE AI TESTING PART
*/
/* 	m_pGrandTichuButton = m_pButtonManager->AddButton("GrandTichuButton.png", [&]()
		{
			//todo: add declare grand tichu logic
			ody::ServiceLocator::GetSoundSystem().PlaySound(11);
		}, { 293, 695 });
	m_pDealCardsButton = m_pButtonManager->AddButton("DealButton.png", [&]()
		{
			//todo: add deal cards logic
			ody::ServiceLocator::GetSoundSystem().PlaySound(0);
		}, { 152, 695 });
	m_pConfirmTradesButton = m_pButtonManager->AddButton("ConfirmButton.png", [&]()
		{
			//todo: add confirm trades logic
			ody::ServiceLocator::GetSoundSystem().PlaySound(0);
		}, { 392, 575 });
	m_pConfirmTradesButton->SetVisible(false); */
}

void HumanPlayer::CalculateHitBoxes()
{
    if (!m_HitBoxesDirty || m_Cards.empty()) return;

    m_CardHitBoxMap.clear();

    const float cardWidth = static_cast<float>(m_RenderPackage.cardTextures[0]->GetSize().x);
    const float cardHeight = static_cast<float>(m_RenderPackage.cardTextures[0]->GetSize().y);

    // Add hitboxes for partially visible cards
    for (size_t i = 0; i < m_Cards.size() - 1; ++i)
    {
        const bool isSelected = std::find(m_SelectedCards.begin(), m_SelectedCards.end(), m_Cards[i]) != m_SelectedCards.end();
        //Move them to the side slightly, so you can see all the cards
        const glm::vec3 cardPosition = m_StartPosition + m_Offset * static_cast<float>(i) + glm::vec3{ m_CardPickupDirection, 0.f } * m_CardPickupAmount * static_cast<float>(isSelected);


        // For partially visible cards, use m_CardSpacing as width
        const float centerX = cardPosition.x + (m_RenderPackage.cardSpacing) / 2.0f;
        const float centerY = cardPosition.y + (cardHeight * m_RenderPackage.cardScale) / 2.0f;

        CardHitbox cardHitBox = CalculateRotatedHitbox(
            centerX,
            centerY,
            m_RenderPackage.cardSpacing, // Use m_CardSpacing for width
            cardHeight * m_RenderPackage.cardScale,
            m_Rotation,
            true
        );

        m_CardHitBoxMap.emplace(m_Cards[i], cardHitBox);
    }

    // Add hitbox for the last, fully visible card
    if (!m_Cards.empty())
    {
        const size_t lastIndex = m_Cards.size() - 1;
        const bool isSelected = std::find(m_SelectedCards.begin(), m_SelectedCards.end(), m_Cards[lastIndex]) != m_SelectedCards.end();
        const glm::vec3 lastCardPosition = m_StartPosition + m_Offset * static_cast<float>(lastIndex) + glm::vec3{ m_CardPickupDirection, 0.f } * m_CardPickupAmount * static_cast<float>(isSelected);

        const float centerX = lastCardPosition.x + (cardWidth * m_RenderPackage.cardScale) / 2.0f;
        const float centerY = lastCardPosition.y + (cardHeight * m_RenderPackage.cardScale) / 2.0f;

        CardHitbox lastCardHitBox = CalculateRotatedHitbox(
            centerX,
            centerY,
            cardWidth * m_RenderPackage.cardScale,
            cardHeight * m_RenderPackage.cardScale,
            m_Rotation,
            false
        );

        m_CardHitBoxMap.emplace(m_Cards[lastIndex], lastCardHitBox);
    }

    m_HitBoxesDirty = false;
}

void HumanPlayer::SelectCardAtMousePosition(const glm::vec2& mousePosition)
{
    for (const auto& [card, hitbox] : m_CardHitBoxMap)
    {
        if (mousePosition.x >= hitbox.x && mousePosition.x <= hitbox.x + hitbox.width &&
            mousePosition.y >= hitbox.y && mousePosition.y <= hitbox.y + hitbox.height)
        {
            //If if it doesn't exist, add it to the selected ones
            const auto cardIt = std::find(m_SelectedCards.begin(), m_SelectedCards.end(), card);
            if (cardIt == m_SelectedCards.end())
            {
                if (card.colour == CC_Mahjong)
				    m_ShowMahjongSelectionTable = true;

                m_SelectedCards.emplace_back(card);
            }
            //Otherwise remove it
            else
            {
                if (card.colour == CC_Mahjong)
                    m_ShowMahjongSelectionTable = false;

                m_SelectedCards.erase(cardIt);
            }

            m_HitBoxesDirty = true;
            return;
        }
    }

    // If we reach here, no card was selected
}

void HumanPlayer::AskForDragon()
{
    // Disable play and pass buttons while asking for dragon
    m_pPlayButton->SetEnabled(false);
    m_pPassButton->SetEnabled(false);

    m_pGiveDragonButtonLeft->SetVisible(true);
    m_pGiveDragonButtonRight->SetVisible(true);
}

void HumanPlayer::GiveDragonToPlayer(const int playerID)
{
    m_pGiveDragonButtonLeft->SetVisible(false);
    m_pGiveDragonButtonRight->SetVisible(false);

    m_pPlayButton->SetEnabled(true);
    m_pPassButton->SetEnabled(true);

    ody::DragonEventData eventData{playerID};
    m_PlayerSubject.EventTriggered(ody::GameEvent::AskForDragon, eventData);
}

CardHitbox HumanPlayer::CalculateRotatedHitbox(float centerX, float centerY, float width, float height, float rotation, bool manualCorrection)
{
    // Convert rotation to radians
    const float rotationRad = glm::radians(rotation);

    // Calculate the four corners of the original rectangle
    glm::vec2 topLeft(-width / 2, -height / 2);
    glm::vec2 topRight(width / 2, -height / 2);
    glm::vec2 bottomLeft(-width / 2, height / 2);
    glm::vec2 bottomRight(width / 2, height / 2);

    // Rotate each corner
    topLeft = glm::rotate(topLeft, rotationRad);
    topRight = glm::rotate(topRight, rotationRad);
    bottomLeft = glm::rotate(bottomLeft, rotationRad);
    bottomRight = glm::rotate(bottomRight, rotationRad);


    // Find the extremes
    const float minX = std::min({ topLeft.x, topRight.x, bottomLeft.x, bottomRight.x });
    const float maxX = std::max({ topLeft.x, topRight.x, bottomLeft.x, bottomRight.x });
    const float minY = std::min({ topLeft.y, topRight.y, bottomLeft.y, bottomRight.y });
    const float maxY = std::max({ topLeft.y, topRight.y, bottomLeft.y, bottomRight.y });

    // Calculate the dimensions of the rotated hitbox
    const float rotatedWidth = maxX - minX;
    const float rotatedHeight = maxY - minY;

    // Calculate the position of the top-left corner
    float rotatedX = centerX + minX;
    float rotatedY = centerY + minY;

    if (rotation == 90.f && manualCorrection)
    {
        rotatedY -= (m_RenderPackage.cardSpacing + 2.f);
        rotatedX += m_RenderPackage.cardSpacing;
    }

    if (rotation == 270.f && manualCorrection)
    {
        rotatedY -= (m_RenderPackage.cardSpacing + 2.f);
        rotatedX += m_RenderPackage.cardSpacing;
    }

    return CardHitbox{ rotatedX, rotatedY, rotatedWidth, rotatedHeight };
}