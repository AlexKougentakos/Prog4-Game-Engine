#include "AIPlayer_MCTS.h"
#include "Tichu.h"
#include "Scenes/TichuScene.h"
#include <Helpers/MCTS.h>
#include <chrono>
#include <imgui.h>

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
    m_DebugInfo = MCTSDebugInfo{}; // Reset debug info
    m_DebugInfo.isThinking = true;
    m_DebugInfo.totalIterations = 100000; // Update this based on your parameters
    
    // Launch the move calculation in a separate thread
    m_MoveFuture = std::async(std::launch::async, [this]() 
    {
        MCTS::GameState rootState{};
        rootState.currentPlayerIndex = static_cast<int8_t>(m_PlayerID);
        m_pScene->FillGameState(rootState);
        
        auto bestState = MCTS::MonteCarloTreeSearch(rootState, 60000, 
            [this](const MCTS::MCTSProgress& progress) 
            {
                // Update debug info
                m_DebugInfo.currentIteration = progress.currentIteration;
                m_DebugInfo.bestMove = progress.bestMove;
                m_DebugInfo.bestMoveScore = progress.bestMoveScore;
                m_DebugInfo.visitCount = progress.visitCount;
            });

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

        m_DebugInfo.isThinking = false;
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
    if (ImGui::CollapsingHeader("MCTS AI Debug", ImGuiTreeNodeFlags_DefaultOpen))
    {
        // Use smaller text to fit more information
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 2));
        
        ImGui::Text("AI Status: %s", m_IsCalculatingMove ? "Thinking..." : "Idle");

        if (m_IsCalculatingMove)
        {
            ImGui::Text("Analyzing moves...");
            
            if (!m_DebugInfo.bestMove.empty())
            {
                ImGui::Text("Best Move:");
                ImGui::Indent(10);
                for (const auto& card : m_DebugInfo.bestMove)
                {
                    ImGui::Text("%s (%d)", 
                        MCTS::GetCardColourString(card.colour).c_str(), 
                        static_cast<int>(card.power));
                }
                ImGui::Unindent(10);

                ImGui::Text("Score: %.2f", m_DebugInfo.bestMoveScore);
                ImGui::Text("Visits: %d", m_DebugInfo.visitCount);
            }

            if (m_DebugInfo.totalIterations > 0)
            {
                float progress = static_cast<float>(m_DebugInfo.currentIteration) / 
                               static_cast<float>(m_DebugInfo.totalIterations);
                ImGui::ProgressBar(progress, ImVec2(-1, 2), 
                    std::to_string(m_DebugInfo.currentIteration).c_str());
            }
        }

        if (m_pTichuGame)
        {
            ImGui::Separator();
            ImGui::Text("Game State:");
            ImGui::Text("Players Left: %d", m_pTichuGame->GetPlayersLeftWhenLastHandPlayed());
            ImGui::Text("Passes: %d", m_pTichuGame->GetPassesInARow());
            
            const auto& combo = m_pTichuGame->GetCurrentStrongestCombination();
            ImGui::Text("Current Combo: %d (%d)", 
                static_cast<int>(combo.combinationType), 
                combo.power);
        }

        static int iterations = 200000;
        ImGui::SliderInt("Iterations", &iterations, static_cast<int>(iterations / 10), static_cast<int>(iterations * 10));
        if (ImGui::Button("Force Calc"))
        {
            if (!m_IsCalculatingMove)
            {
                m_IsCalculatingMove = true;
                StartMoveCalculation();
            }
        }
        
        ImGui::PopStyleVar();
    }
}