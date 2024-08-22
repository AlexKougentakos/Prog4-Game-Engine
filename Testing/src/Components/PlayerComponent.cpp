#include "PlayerComponent.h"

#include <imgui.h>
#include <iostream>
#include <string>
#include <ranges>
#include <SDL_image.h>

#include "Constants.h"
#include "DebugDrawer.h"
#include "GameObject.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "Texture2D.h"
#include "TransformComponent.h"
#include "glm/gtx/rotate_vector.hpp"

PlayerComponent::PlayerComponent(const int playerID, const CardRenderPackage& renderPackage):
	m_PlayerID(playerID),
    m_RenderPackage{ renderPackage }
{

}

void PlayerComponent::Initialize()
{
    CalculateHitBoxes();

	m_pRedLightTexture = ody::ResourceManager::GetInstance().LoadTexture("RedLight.png");
	m_pGreenLightTexture = ody::ResourceManager::GetInstance().LoadTexture("GreenLight.png");

    m_LightSize = { 40.f, 40.f };
}

void PlayerComponent::RenderLights() const
{
	if (m_IsPlaying)
	{
		ody::Renderer::GetInstance().RenderTexture(
			*m_pGreenLightTexture,
			m_LightPosition.x,
			m_LightPosition.y,
			40.f,
			40.f
		);
	}
	else
	{
		ody::Renderer::GetInstance().RenderTexture(
			*m_pRedLightTexture,
			m_LightPosition.x,
			m_LightPosition.y,
			40.f,
			40.f
		);
	}
}

void PlayerComponent::Render() const
{
    if (m_RenderPackage.cardTextures.empty()) return;

    if (m_IsOut) return;

    RenderCards();

    RenderLights();

    if (m_ShowMahjongSelectionTable)
    {
		
    }

    if (!m_ShowCardHitboxes) return;


#ifndef __EMSCRIPTEN__
    //Debug draw the hitboxes
    for (const auto& value : m_CardHitBoxMap | std::views::values)
    {
        ody::DebugDrawer::GetInstance().DrawSquare(value.x, value.y, value.width, value.height);
    }
#endif
}

void PlayerComponent::Update() 
{
    CalculateHitBoxes(); //This has a dirty check so it's not expensive
}

void PlayerComponent::RenderCards() const
{
    const float cardWidth = static_cast<float>(m_RenderPackage.cardTextures[0]->GetSize().x);
    const float cardHeight = static_cast<float>(m_RenderPackage.cardTextures[0]->GetSize().y);

    for (size_t i = 0; i < m_Cards.size(); ++i)
    {
        const bool isSelected = std::find(m_SelectedCards.begin(), m_SelectedCards.end(), m_Cards[i]) != m_SelectedCards.end();

        //Move them to the side slightly, so you can see all the cards
        const glm::vec3 cardPosition = m_StartPosition + m_Offset * static_cast<float>(i) + glm::vec3{ m_CardPickupDirection, 0.f } *m_CardPickupAmount * static_cast<float>(isSelected);

        // Calculate the name of the card texture
        //const size_t textureIndex = m_Cards[i].colour * 13 + m_Cards[i].power - 2;

        const size_t textureIndex = [&]() -> size_t
            {
                //This is the order in which they are added inside the CreateDeck() function in the TichuScene
                switch (m_Cards[i].colour)
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
                    return m_Cards[i].colour * 13 + m_Cards[i].power - 2; //Calculate the texture index using the colour to find the right tile image
                }
            }();


#ifdef _DEBUG
            if (m_ShowCardBacks && m_PlayerID != 0)
            {
                ody::Renderer::GetInstance().RenderTexture(
                    *m_RenderPackage.cardBack,
                    cardPosition.x,
                    cardPosition.y,
                    cardWidth,
                    cardHeight,
                    m_Rotation,
                    m_RenderPackage.cardScale,
                    SDL_FLIP_NONE
                );
            }
            else
#endif
                // Render the texture with rotation
                ody::Renderer::GetInstance().RenderTexture(
                    *m_RenderPackage.cardTextures[textureIndex],
                    cardPosition.x,
                    cardPosition.y,
                    cardWidth,
                    cardHeight,
                    m_Rotation,
                    m_RenderPackage.cardScale,
                    SDL_FLIP_NONE
                );
    }
}

void PlayerComponent::CalculateHitBoxes()
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

void PlayerComponent::SelectCardAtMousePosition(const glm::vec2& mousePosition)
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

void PlayerComponent::SetCards(const std::vector<Card>& newCards)
{

    m_Cards.clear();
    m_SelectedCards.clear();
    m_CardHitBoxMap.clear();
    m_HitBoxesDirty = true;

    m_Cards = newCards;

    CalculateRenderingParameters();
}

void PlayerComponent::PlayedSelectedCards()
{
    // Remove selected cards from m_Cards
    std::erase_if(m_Cards, [this](const Card& card) 
                {
	                return std::find(m_SelectedCards.begin(), m_SelectedCards.end(), card) != m_SelectedCards.end();
                });

    // Clear the selected cards
    m_SelectedCards.clear();
    m_HitBoxesDirty = true;
    m_ShowMahjongSelectionTable = false;
    if (m_Cards.empty()) m_IsOut = true;
    CalculateRenderingParameters();
}

void PlayerComponent::Pass()
{
    m_SelectedCards.clear();
    m_HitBoxesDirty = true;
    CalculateRenderingParameters();
}

void PlayerComponent::CalculateRenderingParameters()
{
    const float cardHeight = static_cast<float>(m_RenderPackage.cardTextures[0]->GetSize().y);
    const float cardWidth = static_cast<float>(m_RenderPackage.cardTextures[0]->GetSize().x);

    m_ScreenWidth = ody::constants::g_ScreenWidth;
    m_ScreenHeight = ody::constants::g_ScreenHeight;
        
    const float stackWidth = m_RenderPackage.cardSpacing * (m_Cards.size() - 1) + cardWidth * m_RenderPackage.cardScale;

    switch (m_PlayerID) 
    {
    case 0: // Bottom
        m_StartPosition = glm::vec3(m_ScreenWidth / 2 - stackWidth / 2, m_ScreenHeight - m_RenderPackage.cardScale * cardHeight, 0);
        m_Offset = glm::vec3(m_RenderPackage.cardSpacing, 0, 0);
        m_Rotation = 0.0f;
        m_CardPickupDirection = { 0,-1 };
        m_LightPosition = { m_StartPosition.x - m_LightSize.x * 1.5, m_StartPosition.y + (cardHeight * m_RenderPackage.cardScale) / 2 - m_LightSize.y / 2};
        break;
    case 1: // Left
        m_StartPosition = glm::vec3(15.f, (m_ScreenHeight + m_RenderPackage.pointDisplayHeight) / 2 - stackWidth / 2 - 35.f, 0);
        m_Offset = glm::vec3(0, m_RenderPackage.cardSpacing, 0);
        m_Rotation = 90.0f;
        m_CardPickupDirection = { 1,0};
        m_LightPosition = { (cardHeight * m_RenderPackage.cardScale) / 2 - m_LightSize.x / 2, m_StartPosition.y - m_LightSize.y };
        break;
    case 2: // Top
        m_StartPosition = glm::vec3(m_ScreenWidth / 2 - stackWidth / 2, m_RenderPackage.pointDisplayHeight, 0);
        m_Offset = glm::vec3(m_RenderPackage.cardSpacing, 0, 0);
        m_Rotation = 180.0f;
        m_CardPickupDirection = { 0,1 };
		m_LightPosition = { m_StartPosition.x + m_LightSize.x / 2 + stackWidth, m_StartPosition.y + (cardHeight * m_RenderPackage.cardScale) / 2 - m_LightSize.y / 2 };
        break;
    case 3: // Right
        m_StartPosition = glm::vec3(m_ScreenWidth - m_RenderPackage.cardScale * cardHeight + 20.f, (m_ScreenHeight + m_RenderPackage.pointDisplayHeight) / 2 - stackWidth / 2 - 35.f, 0);
        m_Offset = glm::vec3(0, m_RenderPackage.cardSpacing, 0);
        m_Rotation = 270.0f;
        m_CardPickupDirection = { -1,0 };
		m_LightPosition = { m_StartPosition.x + m_LightSize.x / 2, m_StartPosition.y + stackWidth + m_LightSize.y };
        break;
    default: ;
    }
}

void PlayerComponent::OnGui()
{
    const std::string uniqueID = "IndentedBox_" + std::to_string(m_PlayerID);
    ImGui::BeginChild(uniqueID.c_str(), ImVec2(0, 60), true, ImGuiWindowFlags_None);
    ImGui::Text(std::string("Player " + std::to_string(m_PlayerID)).c_str());
    ImGui::Separator();

    ImGui::Text(std::string("Points Held: " + std::to_string(m_HoldingPoints)).c_str());

    ImGui::EndChild();

}