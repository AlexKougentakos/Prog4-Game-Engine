#include "PlayerComponent.h"

#include <string>

#include "Constants.h"
#include "GameObject.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "Texture2D.h"
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

    const float cardWidth = static_cast<float>(m_CardTextures[0]->GetSize().x);
    const float cardHeight = static_cast<float>(m_CardTextures[0]->GetSize().y);

    for (size_t i = 0; i < m_Cards.size(); ++i)
    {
        const glm::vec3 cardPosition = m_StartPosition + m_Offset * static_cast<float>(i);
        
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
            1.5f,
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
    const float cardHeight = static_cast<float>(m_CardTextures[0]->GetSize().y);
    const float cardWidth = static_cast<float>(m_CardTextures[0]->GetSize().x);
    constexpr float cardSpacing = 20.0f;

    m_ScreenWidth = ody::constants::g_ScreenWidth;
    m_ScreenHeight = ody::constants::g_ScreenHeight;

        
    const float stackWidth = cardSpacing * (m_Cards.size() - 1) + cardWidth * 1.5f;

    switch (m_PlayerID) {
    case 0: // Bottom
        m_StartPosition = glm::vec3(m_ScreenWidth / 2 - stackWidth / 2, m_ScreenHeight - 1.5f * cardHeight, 0); //Multiply by 1.5 because we're scaling it by 1.5
        m_Offset = glm::vec3(cardSpacing, 0, 0);
        m_Rotation = 0.0f;
        break;
    case 1: // Left
        m_StartPosition = glm::vec3(15.f, m_ScreenHeight / 2 - stackWidth / 2, 0);
        m_Offset = glm::vec3(0, cardSpacing, 0);
        m_Rotation = 90.0f;
        break;
    case 2: // Top
        m_StartPosition = glm::vec3(m_ScreenWidth / 2 - stackWidth / 2, 0, 0);
        m_Offset = glm::vec3(cardSpacing, 0, 0);
        m_Rotation = 180.0f;
        break;
    case 3: // Right
        m_StartPosition = glm::vec3(m_ScreenWidth - 1.5f * cardHeight + 20.f, m_ScreenHeight / 2 - stackWidth / 2, 0); //Multiply by 1.5 because we're scaling it by 1.5
        m_Offset = glm::vec3(0, cardSpacing, 0);
        m_Rotation = 270.0f;
        break;
    default: ;  
    }
}