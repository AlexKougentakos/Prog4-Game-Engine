#include "AIPlayer_MCTS.h"
#include "Tichu.h"
#include "Scenes/TichuScene.h"
#include <Helpers/MCTS.h>
#include <chrono>

AIPlayer_MCTS::AIPlayer_MCTS(const int playerID, const CardRenderPackage &renderPackage) :
	PlayerComponent(playerID, renderPackage)
{

}

void AIPlayer_MCTS::Initialize()
{
    PlayerComponent::Initialize();
}

void AIPlayer_MCTS::Update()
{
    PlayerComponent::Update();

    if (m_IsPlaying)
    {
        MakeMove();
    }
}

void AIPlayer_MCTS::AskForDragon()
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

void AIPlayer_MCTS::MakeMove()
{
    MCTS::GameState rootState{};
    rootState.currentPlayerIndex = m_PlayerID;
    m_pScene->FillGameState(rootState);
    
    // Enable/disable debug logging here
    MCTS::DebugLogger::Enable(false);
    
    // Time how long it takes to run the MCTS
    auto startTime = std::chrono::high_resolution_clock::now();
    auto bestState = MCTS::MonteCarloTreeSearch(rootState, 100);
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    std::cout << "MCTS took " << duration.count() << " milliseconds to run." << std::endl;

    auto currentCards = rootState.playerHands[m_PlayerID];
    auto bestPlay = bestState.playerHands[m_PlayerID];
    std::vector<Card> cardsToPlay{};

    //Get the difference between the two vectors
    for (const auto& card : currentCards)
    {
        if (std::find(bestPlay.begin(), bestPlay.end(), card) == bestPlay.end())
        {
            cardsToPlay.push_back(card);
        }
    }

    if (cardsToPlay.empty())
    {
        Pass();
        return;
    }

    m_SelectedCards = cardsToPlay;
    PlayedSelectedCards();
}