﻿#include "AIPlayer_DMCTS.h"
#include "Tichu.h"
#include "Scenes/TichuScene.h"
#include <Helpers/MCTS.h>
#include <chrono>

#include "PerformanceTimer.h"

AIPlayer_DMCTS::AIPlayer_DMCTS(const int playerID, const CardRenderPackage &renderPackage, const int iterations, const int determinizations) :
	PlayerComponent(playerID, renderPackage),
    m_Iterations(iterations),
    m_Determinizations(determinizations)
{
    
}

void AIPlayer_DMCTS::Initialize()
{
    PlayerComponent::Initialize();
}

void AIPlayer_DMCTS::Update()
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

void AIPlayer_DMCTS::StartMoveCalculation()
{
    m_IsCalculatingMove = true;
    
    
    // Launch the move calculation in a separate thread
    m_MoveFuture = std::async(std::launch::async, [this]() 
    {
        ody::PerformanceTimer timer{};
        timer.Start();
        bool forcedMove{false};
        
        MCTS::GameState rootState{};
        rootState.currentPlayerIndex = static_cast<int8_t>(m_PlayerID);
        std::vector<Card> remainingCards{};
        std::vector<Card> playerCards{};
        m_pScene->FillDeterminizedStates(rootState, remainingCards);

        std::vector<MCTS::GameState> moves{};
        rootState.GetPossibleGameStates(rootState, moves);
        if (moves.size() == 1)
            forcedMove = true;
        

#ifdef _DEBUG
        auto bestState = MCTS::DeterminizedTreeSearch(rootState, 500, 10, remainingCards);
#else
        auto bestState = MCTS::DeterminizedTreeSearch(rootState, m_Iterations, m_Determinizations, remainingCards);
#endif
        
        auto currentCards = rootState.playerHands[m_PlayerID];
        auto bestPlay = bestState.playerHands[m_PlayerID];
        std::vector<Card> cardsToPlay{};

        for (const auto& card : currentCards)
        {
            if (std::find(bestPlay.begin(), bestPlay.end(), card) == bestPlay.end())
            {
                cardsToPlay.push_back(card);
            }
        }
        
        if (!cardsToPlay.empty())
        {
            [[maybe_unused]] auto test = m_pTichuGame->CreateCombination(cardsToPlay);
            if (test.combinationType == CombinationType::CT_Invalid && test.numberOfCards != 0)
            {
                __debugbreak();
            }
        }

        timer.Stop();

        if (!forcedMove) // Don't add the time if the move was forced (1 possible move). It will skew the results
            m_RoundMoveTimeMap[m_RoundCounter][m_MoveCounter] = timer.GetElapsedMilliseconds();
        
        return cardsToPlay;
    });
}

void AIPlayer_DMCTS::ExecuteMove(const std::vector<Card>& cardsToPlay)
{
    if (cardsToPlay.empty())
    {
        Pass();
        return;
    }

    m_SelectedCards = cardsToPlay;
    PlayedSelectedCards();
}

void AIPlayer_DMCTS::AskForDragon()
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

void AIPlayer_DMCTS::OnGuiMCTS()
{
   
}