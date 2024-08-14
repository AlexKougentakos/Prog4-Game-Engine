#include "PlayerComponent.h"

#include <string>

#include "Constants.h"
#include "GameObject.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "TransformComponent.h"

PlayerComponent::PlayerComponent(const int playerID, const std::vector<Card>& cards):
	m_PlayerID(playerID)
{
	m_Cards = cards;
}

void PlayerComponent::Initialize()
{
    LoadCardTextures();
    CalculateRenderingParameters();
}

void PlayerComponent::Render() const
{
    if (m_CardTextures.empty()) return;

    constexpr float cardWidth = 50.0f;
    constexpr float cardHeight = 70.0f;

    // Adjust start position to center the hand
    const glm::vec3 adjustedStartPosition = m_StartPosition - m_Offset * (static_cast<float>(m_Cards.size() - 1) / 2.0f);

    for (size_t i = 0; i < m_Cards.size(); ++i)
    {
        const glm::vec3 cardPosition = adjustedStartPosition + m_Offset * static_cast<float>(i);
        
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
            1.0f,
            SDL_FLIP_NONE
        );
    }
}

void PlayerComponent::LoadCardTextures()
{
    m_CardTextures.clear();
    for (int i = 0; i < 52; ++i) //todo: make this 56 cards later
    {
        std::string cardName = std::to_string(i);
        std::string paddedCardName = std::string(3 - cardName.length(), '0') + cardName;
        std::string completePath = "Cards/tile" + paddedCardName + ".png";
        m_CardTextures.push_back(ody::ResourceManager::GetInstance().LoadTexture(completePath));
    }
}

void PlayerComponent::CalculateRenderingParameters()
{
    constexpr float cardHeight = 70.0f; // Adjust based on your card dimensions
    constexpr float cardSpacing = 20.0f;

    m_ScreenWidth = ody::constants::g_ScreenWidth;
    m_ScreenHeight = ody::constants::g_ScreenHeight;

    switch (m_PlayerID)
    {
        case 0: // Bottom
            m_StartPosition = glm::vec3(m_ScreenWidth / 2, m_ScreenHeight - cardHeight / 2, 0);
            m_Offset = glm::vec3(cardSpacing, 0, 0);
            m_Rotation = 0.0f;
            break;
        case 1: // Left
            m_StartPosition = glm::vec3(cardHeight / 2, m_ScreenHeight / 2, 0);
            m_Offset = glm::vec3(0, -cardSpacing, 0);
            m_Rotation = 90.0f;
            break;
        case 2: // Top
            m_StartPosition = glm::vec3(m_ScreenWidth / 2, cardHeight / 2, 0);
            m_Offset = glm::vec3(-cardSpacing, 0, 0);
            m_Rotation = 180.0f;
            break;
        case 3: // Right
            m_StartPosition = glm::vec3(m_ScreenWidth - cardHeight / 2, m_ScreenHeight / 2, 0);
            m_Offset = glm::vec3(0, cardSpacing, 0);
            m_Rotation = 270.0f;
            break;
        default: ;
    }
}
