#include "AIPlayer.h"
#include "Tichu.h"

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

void AIPlayer::MakeRandomMove()
{
    // 30% chance to pass if there are cards on the table
    if (!m_CardsOnTop.empty() && (rand() % 100) < 30)
    {
        m_SelectedCards.clear();
        Pass();
        return;
    }

    // Clear previous selection
    m_SelectedCards.clear();

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
        
        std::random_shuffle(indices.begin(), indices.end());

        for (int i = 0; i < numCards; ++i)
        {
            m_SelectedCards.push_back(m_Cards[indices[i]]);
        }

        // If we found a valid combination, try to play it
        std::sort(m_SelectedCards.begin(), m_SelectedCards.end());
        if (m_pTichuGame->CreateCombination(m_SelectedCards).combinationType != CombinationType::CT_Invalid)
        {
            if (m_pScene)
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