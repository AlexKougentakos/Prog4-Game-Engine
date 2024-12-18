#include "AIPlayer_MCTS.h"
#include "Tichu.h"
#include "Scenes/TichuScene.h"
#include <Helpers/MCTS.h>
#include <chrono>
#include <imgui.h>

AIPlayer_MCTS::AIPlayer_MCTS(const int playerID, const CardRenderPackage &renderPackage, const int iterations) :
	PlayerComponent(playerID, renderPackage),
    m_Iterations(iterations)
{
    
}

void AIPlayer_MCTS::Initialize()
{
    PlayerComponent::Initialize();
}

void AIPlayer_MCTS::Update()
{
    PlayerComponent::Update();

    if (m_IsPlaying && !m_IsCalculatingMove)
    {
        StartMoveCalculation();
    }
    
    // Check if move calculation is complete
    if (m_IsCalculatingMove && m_MoveFuture.valid())
    {
        // Check if the future is ready without blocking
        if (m_MoveFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
        {
            // Get the calculated move
            std::vector<Card> cardsToPlay = m_MoveFuture.get();
            ExecuteMove(cardsToPlay);
            m_IsCalculatingMove = false;
        }
    }
}

void AIPlayer_MCTS::StartMoveCalculation()
{
    m_IsCalculatingMove = true;
    
    // Launch the move calculation in a separate thread
    m_MoveFuture = std::async(std::launch::async, [this]() 
    {
        MCTS::GameState rootState{};
        rootState.currentPlayerIndex = static_cast<int8_t>(m_PlayerID);
        m_pScene->FillGameState(rootState);

        
#ifdef _DEBUG
        auto bestState = MCTS::MonteCarloTreeSearch(rootState, 50);
#else
        std::cout << "Iterations: " << m_Iterations << std::endl;
        auto bestState = MCTS::MonteCarloTreeSearch(rootState, m_Iterations);
#endif

        auto currentCards = rootState.playerHands[m_PlayerID];
        auto bestPlay = bestState.playerHands[m_PlayerID];
        std::vector<Card> cardsToPlay{};

        // Get the difference between the two vectors
        for (const auto& card : currentCards)
        {
            if (std::find(bestPlay.begin(), bestPlay.end(), card) == bestPlay.end())
            {
                cardsToPlay.push_back(card);
            }
        }
        
        return cardsToPlay;
    });
}

void AIPlayer_MCTS::ExecuteMove(const std::vector<Card>& cardsToPlay)
{
    if (cardsToPlay.empty())
    {
        Pass();
        return;
    }

    m_SelectedCards = cardsToPlay;
    PlayedSelectedCards();
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

void AIPlayer_MCTS::OnGuiMCTS()
{
   
}