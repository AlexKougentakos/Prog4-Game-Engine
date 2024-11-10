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
#include <random>
#include <Tichu.h>
#include "../Scenes/TichuScene.h"

PlayerComponent::PlayerComponent(const int playerID, const CardRenderPackage& renderPackage):
	m_PlayerID(playerID),
    m_RenderPackage{ renderPackage }

{

}

void PlayerComponent::Initialize()
{
	m_pRedLightTexture = ody::ResourceManager::GetInstance().LoadTexture("RedLight.png");
	m_pGreenLightTexture = ody::ResourceManager::GetInstance().LoadTexture("GreenLight.png");

	m_pTichuTokenTexture = ody::ResourceManager::GetInstance().LoadTexture("TichuLight.png");
	m_pGrandTichuTokenTexture = ody::ResourceManager::GetInstance().LoadTexture("GrandTichuLight.png");

    m_LightSize = { 40.f, 40.f };
}


void PlayerComponent::Render() const
{
    if (m_RenderPackage.cardTextures.empty()) return;

    if (m_IsOut) return;

    RenderCards();

    RenderLights();

    if (m_DeclaredTichu)
    {
        const auto offset = m_CardPickupDirection * glm::vec2{ -40.f, -40.f };
        ody::Renderer::GetInstance().RenderTexture(
            *m_pTichuTokenTexture,
            m_LightPosition.x + offset.x,
            m_LightPosition.y + offset.y,
            40.f,
            40.f
        );
    }
	else if (m_DeclaredGrandTichu)
	{
		const auto offset = m_CardPickupDirection * glm::vec2{ -40.f, -40.f };
		ody::Renderer::GetInstance().RenderTexture(
			*m_pGrandTichuTokenTexture,
			m_LightPosition.x + offset.x,
			m_LightPosition.y + offset.y,
			40.f,
			40.f
		);
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


#ifdef NDEBUG
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
    // Create event data with selected cards
    ody::CardEventData eventData(m_SelectedCards);
    
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

    m_PlayerSubject.EventTriggered(ody::GameEvent::PlayCards, eventData);
}

void PlayerComponent::Pass()
{
    m_SelectedCards.clear();
    m_HitBoxesDirty = true;
    CalculateRenderingParameters();

    m_PlayerSubject.EventTriggered(ody::GameEvent::Pass);
}

void PlayerComponent::SetSceneReference(TichuScene* scene)
{
	m_pScene = scene;
	m_PlayerSubject.AddObserver(m_pScene);
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
    ImGui::Text("%s", std::string("Player " + std::to_string(m_PlayerID)).c_str());
    ImGui::Separator();

    ImGui::Text("%s", std::string("Points Held: " + std::to_string(m_HoldingPoints)).c_str());
    ImGui::Text("In Turn: ");
    ImGui::SameLine();
    ImGui::TextColored(m_IsPlaying ? ImVec4(0.0f, 1.0f, 0.0f, 1.0f) : ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "%s", m_IsPlaying ? "Yes" : "No");

    ImGui::EndChild();

}