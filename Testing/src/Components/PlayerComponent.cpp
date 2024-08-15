#include "PlayerComponent.h"

#include <imgui.h>
#include <iostream>
#include <string>
#include <ranges>

#include "Constants.h"
#include "DebugDrawer.h"
#include "GameObject.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "Texture2D.h"
#include "TransformComponent.h"
#include "glm/gtx/rotate_vector.hpp"

PlayerComponent::PlayerComponent(const int playerID, const std::vector<Card>& cards):
	m_PlayerID(playerID)
{
	m_Cards = cards;
}

void PlayerComponent::SelectCardAtMousePosition(const glm::vec2& mousePosition)
{
    for (const auto& [card, hitbox] : m_CardHitBoxMap)
    {
        if (mousePosition.x >= hitbox.x && mousePosition.x <= hitbox.x + hitbox.width &&
            mousePosition.y >= hitbox.y && mousePosition.y <= hitbox.y + hitbox.height)
        {
            //If if it doesn't exist, add it to the selected ones
            const auto cardIt = std::ranges::find(m_SelectedCards, card);
            if (cardIt == m_SelectedCards.end())
            {
                m_SelectedCards.emplace_back(card);
            }
            //Otherwise remove it
            else m_SelectedCards.erase(cardIt);

            m_HitBoxesDirty = true;
            return;
        }
    }

    // If we reach here, no card was selected
}

void PlayerComponent::Initialize()
{
    LoadCardTextures();
    CalculateRenderingParameters();

    CalculateHitBoxes();
}

void PlayerComponent::Render() const
{
    if (m_CardTextures.empty()) return;

    const float cardWidth = static_cast<float>(m_CardTextures[0]->GetSize().x);
    const float cardHeight = static_cast<float>(m_CardTextures[0]->GetSize().y);

    for (size_t i = 0; i < m_Cards.size(); ++i)
    {
        const bool isSelected = std::ranges::find(m_SelectedCards, m_Cards[i]) != m_SelectedCards.end();
        //Move them to the side slightly, so you can see all the cards
        const glm::vec3 cardPosition = m_StartPosition + m_Offset * static_cast<float>(i) + glm::vec3{ m_CardPickupDirection, 0.f } * m_CardPickupAmount * static_cast<float>(isSelected);
        
        // Calculate the name of the card texture
        const size_t textureIndex = m_Cards[i].colour * 13 + m_Cards[i].power - 2;
        
        // Render the texture with rotation
        ody::Renderer::GetInstance().RenderTexture(
            *m_CardTextures[textureIndex],
            cardPosition.x,
            cardPosition.y,
            cardWidth,
            cardHeight,
            m_Rotation,
            m_CardScale,
            SDL_FLIP_NONE
        );
    }

    if (!m_ShowCardHitboxes) return;

    //Debug draw the hitboxes
    for (const auto& value : m_CardHitBoxMap | std::views::values) 
    {
        ody::DebugDrawer::GetInstance().DrawSquare(value.x, value.y, value.width, value.height);
    }
}

void PlayerComponent::Update() 
{
    CalculateHitBoxes(); //This has a dirty check so it's not expensive
}

void PlayerComponent::CalculateHitBoxes()
{
    if (!m_HitBoxesDirty) return;

    m_CardHitBoxMap.clear();

    const float cardWidth = static_cast<float>(m_CardTextures[0]->GetSize().x);
    const float cardHeight = static_cast<float>(m_CardTextures[0]->GetSize().y);

    // Add hitboxes for partially visible cards
    for (size_t i = 0; i < m_Cards.size() - 1; ++i)
    {
        const bool isSelected = std::ranges::find(m_SelectedCards, m_Cards[i]) != m_SelectedCards.end();
        //Move them to the side slightly, so you can see all the cards
        const glm::vec3 cardPosition = m_StartPosition + m_Offset * static_cast<float>(i) + glm::vec3{ m_CardPickupDirection, 0.f } * m_CardPickupAmount * static_cast<float>(isSelected);


        // For partially visible cards, use m_CardSpacing as width
        const float centerX = cardPosition.x + (m_CardSpacing) / 2.0f;
        const float centerY = cardPosition.y + (cardHeight * m_CardScale) / 2.0f;

        CardHitbox cardHitBox = CalculateRotatedHitbox(
            centerX,
            centerY,
            m_CardSpacing, // Use m_CardSpacing for width
            cardHeight * m_CardScale,
            m_Rotation,
            true
        );

        m_CardHitBoxMap.emplace(m_Cards[i], cardHitBox);
    }

    // Add hitbox for the last, fully visible card
    if (!m_Cards.empty())
    {
        const size_t lastIndex = m_Cards.size() - 1;
        const bool isSelected = std::ranges::find(m_SelectedCards, m_Cards[lastIndex]) != m_SelectedCards.end();
        const glm::vec3 lastCardPosition = m_StartPosition + m_Offset * static_cast<float>(lastIndex) + glm::vec3{ m_CardPickupDirection, 0.f } * m_CardPickupAmount * static_cast<float>(isSelected);

        const float centerX = lastCardPosition.x + (cardWidth * m_CardScale) / 2.0f;
        const float centerY = lastCardPosition.y + (cardHeight * m_CardScale) / 2.0f;

        CardHitbox lastCardHitBox = CalculateRotatedHitbox(
            centerX,
            centerY,
            cardWidth * m_CardScale,
            cardHeight * m_CardScale,
            m_Rotation,
            false
        );

        m_CardHitBoxMap.emplace(m_Cards[lastIndex], lastCardHitBox);
    }

    m_HitBoxesDirty = false;
}

CardHitbox PlayerComponent::CalculateRotatedHitbox(float centerX, float centerY, float width, float height, float rotation, bool manualCorrection)
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
        rotatedY -= (m_CardSpacing + 2.f);
        rotatedX += m_CardSpacing;
    }

    if (rotation == 270.f && manualCorrection)
    {
        rotatedY -= (m_CardSpacing + 2.f);
        rotatedX += m_CardSpacing;
    }


    return CardHitbox{ rotatedX, rotatedY, rotatedWidth, rotatedHeight };
}

void PlayerComponent::LoadCardTextures()
{
    m_CardTextures.clear();
    for (int i = 0; i < 52; ++i) //todo: make this 56 cards later
    {
        std::string cardName = std::to_string(i);
        std::string paddedCardName = std::string(3 - cardName.length(), '0') + cardName;
        std::string completePath = "Cards/tile" + paddedCardName + ".png";
        m_CardTextures.emplace_back(ody::ResourceManager::GetInstance().LoadTexture(completePath));
    }
}

void PlayerComponent::CalculateRenderingParameters()
{
    const float cardHeight = static_cast<float>(m_CardTextures[0]->GetSize().y);
    const float cardWidth = static_cast<float>(m_CardTextures[0]->GetSize().x);
   

    m_ScreenWidth = ody::constants::g_ScreenWidth;
    m_ScreenHeight = ody::constants::g_ScreenHeight;

        
    const float stackWidth = m_CardSpacing * (m_Cards.size() - 1) + cardWidth * m_CardScale;

    switch (m_PlayerID) 
    {
    case 0: // Bottom
        m_StartPosition = glm::vec3(m_ScreenWidth / 2 - stackWidth / 2, m_ScreenHeight - m_CardScale * cardHeight, 0); //Multiply by 1.5 because we're scaling it by 1.5
        m_Offset = glm::vec3(m_CardSpacing, 0, 0);
        m_Rotation = 0.0f;
        m_CardPickupDirection = { 0,-1 };
        break;
    case 1: // Left
        m_StartPosition = glm::vec3(15.f, m_ScreenHeight / 2 - stackWidth / 2, 0);
        m_Offset = glm::vec3(0, m_CardSpacing, 0);
        m_Rotation = 90.0f;
        m_CardPickupDirection = { 1,0};
        break;
    case 2: // Top
        m_StartPosition = glm::vec3(m_ScreenWidth / 2 - stackWidth / 2, 0, 0);
        m_Offset = glm::vec3(m_CardSpacing, 0, 0);
        m_Rotation = 180.0f;
        m_CardPickupDirection = { 0,1 };
        break;
    case 3: // Right
        m_StartPosition = glm::vec3(m_ScreenWidth - m_CardScale * cardHeight + 20.f, m_ScreenHeight / 2 - stackWidth / 2, 0); //Multiply by 1.5 because we're scaling it by 1.5
        m_Offset = glm::vec3(0, m_CardSpacing, 0);
        m_Rotation = 270.0f;
        m_CardPickupDirection = { -1,0 };
        break;
    default: ;  
    }
}

void PlayerComponent::OnGui()
{
    
}