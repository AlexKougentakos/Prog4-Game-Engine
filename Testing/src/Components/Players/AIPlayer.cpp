#include "AIPlayer.h"
#include "Tichu.h"
#include "Scenes/TichuScene.h"
#include <algorithm>
#include <random>

AIPlayer::AIPlayer(const int playerID, const CardRenderPackage &renderPackage) :
	PlayerComponent(playerID, renderPackage)
{

}

void AIPlayer::Initialize()
{
    PlayerComponent::Initialize();
}

void AIPlayer::Update()
{
    PlayerComponent::Update();

    if (m_IsPlaying)
    {
        // Add a small delay before AI makes a move (optional)
        static float aiDelay = 1.0f;
        static float currentDelay = 0.0f;
        currentDelay += 0.016f; // Assuming 60 FPS

        if (currentDelay >= aiDelay)
        {
            currentDelay = 0.0f;
            MakeRandomMove();
        }
    }
}

void AIPlayer::AskForDragon()
{
    // Determine valid opponent IDs based on this AI's player ID
    int targetPlayer;
    if (m_PlayerID == 1 || m_PlayerID == 3)
    {
        // Players 1 and 3 can give to players 0 or 2
        targetPlayer = (rand() % 2) * 2; // This will give either 0 or 2
    }
    else // m_PlayerID is 2
    {
        // Player 2 can give to players 1 or 3
        targetPlayer = ((rand() % 2) * 2) + 1; // This will give either 1 or 3
    }

    ody::DragonEventData eventData{targetPlayer};
    m_PlayerSubject.EventTriggered(ody::GameEvent::AskForDragon, eventData);
}


void AIPlayer::MakeRandomMove()
{
    // Clear previous selection
    m_SelectedCards.clear();

    // First check if we have the dragon and can play it
    auto dragonIt = std::find_if(m_Cards.begin(), m_Cards.end(), 
        [](const Card& card) { return card.colour == CardColour::CC_Dragon; });

    if (dragonIt != m_Cards.end())
    {
        m_SelectedCards.push_back(*dragonIt);
        Combination combination = m_pTichuGame->CreateCombination(m_SelectedCards);
        if (combination.combinationType != CombinationType::CT_Invalid &&
            m_pTichuGame->GetCurrentStrongestCombination().power < combination.power)
        {
            PlayedSelectedCards();
            return;
        }
        m_SelectedCards.clear();
    }

    // 30% chance to pass if you are allowed to
    if (m_pTichuGame->GetCurrentStrongestCombination().combinationType != CombinationType::CT_Invalid 
        && (rand() % 100) < 30)
    {
        Pass();
        return;
    }

    // Try to make a valid move up to 10 times
    for (int attempt = 0; attempt < 10; ++attempt)
    {
        // Randomly select 1-5 cards
        int numCards = (rand() % 5) + 1;
        numCards = std::min(numCards, static_cast<int>(m_Cards.size()));

        // Randomly select cards
        std::vector<size_t> indices;
        for (size_t i = 0; i < m_Cards.size(); ++i)
            indices.push_back(i);
        
        std::random_device rd;
        std::mt19937 gen(rd());

        std::shuffle(indices.begin(), indices.end(), gen);

        for (int i = 0; i < numCards; ++i)
        {
            m_SelectedCards.push_back(m_Cards[indices[i]]);
        }

        // If we found a valid combination, try to play it
        std::sort(m_SelectedCards.begin(), m_SelectedCards.end());
        Combination combination = m_pTichuGame->CreateCombination(m_SelectedCards);
        if (combination.combinationType != CombinationType::CT_Invalid)
        {
            if (m_pScene && m_pTichuGame->GetCurrentStrongestCombination().power < combination.power)
            {
                PlayedSelectedCards();
                return;
            }
        }

        // If not valid or couldn't play, clear and try again
        m_SelectedCards.clear();
    }
    
    Pass();
}