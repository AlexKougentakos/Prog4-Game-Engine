#pragma once

#include <vector>
#include <memory>

#include "PlayerComponent.h" // For the card structs
#include "Tichu.h"
namespace MCTS
{
    class DebugLogger 
    {
    public:
        static void Enable(bool enable) { s_Enabled = enable; }
        static bool IsEnabled() { return s_Enabled; }
        
        template<typename... Args>
        static void Log(const char* format, Args... args) 
        {
            if (s_Enabled) 
            {
                printf(format, args...);
            }
        }
    private:
        static bool s_Enabled;
    };

struct GameState 
{
    std::array<std::vector<Card>, 4> playerHands{};         // Each player's hand
    int16_t teamScore{};                                    // The score of the team that the current player is on
    int16_t opponentTeamScore{};                            // The score of the opposing team
    Combination currentCombination{};                      // The current strongest combination
    std::array<int8_t, 4> scores{};                         // The score, of the cards that each player has collected
    uint8_t currentPlayerIndex{};                           // Current player's index
    bool tichuCalled[4] = {false};                          // Whether each player called Tichu
    bool grandTichuCalled[4] = {false};                     // Whether each player called Grand Tichu

    // Constructor to initialize the game state
    GameState() : currentPlayerIndex(0) 
    {
        scores.fill(0);
    }

    bool IsTerminal() const;
/*     bool IsTerminal() const 
    {
        // Game ends when a team reaches a specific score or all tricks are completed
        return std::any_of(scores.begin(), scores.end(), [](int score) { return score >= 500; });
    } */

    double GetReward(int player) const;

    void GetPossibleGameStates(const GameState& currentState, std::vector<GameState>& moves) const;

    int GetCurrentPlayer() const { return currentPlayerIndex; }

    void PrintState() const;

    bool operator==(const GameState& other) const
    {
        return playerHands == other.playerHands &&
               teamScore == other.teamScore &&
               opponentTeamScore == other.opponentTeamScore &&
               scores == other.scores &&
               currentPlayerIndex == other.currentPlayerIndex;
    }
};

class Node 
{
public:
    GameState state;
    int player;
    int visitCount;
    double totalValue;
    std::vector<std::unique_ptr<Node>> children;
    Node* parent;

    Node(const GameState& state, int player, Node* parent = nullptr);

    bool IsFullyExpanded() const;
    bool IsLeaf() const;
    Node* BestChild(double explorationWeight) const;
    Node* AddChild(const GameState& childState, int childPlayer);
};

GameState MonteCarloTreeSearch(const GameState& rootState, int iterations);

}