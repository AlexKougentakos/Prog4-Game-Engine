#pragma once

#include <array>
#include <vector>
#include <memory>
#include <functional>

#include "Components/PlayerComponent.h" // For the card structs
#include "Tichu.h"

namespace MCTS
{
    struct GameState 
    {
        std::array<std::vector<Card>, 4> playerHands{};         // Each player's hand
        int16_t teamScore{};                                    // The score of the team that the current player is on
        int16_t opponentTeamScore{};                            // The score of the opposing team
        Combination currentCombination{};                       // The current strongest combination
        std::array<int8_t, 4> scores{};                         // The score of the cards that each player has collected
        uint8_t currentPlayerIndex{};                           // Current player's index
        bool tichuCalled[4] = {false};                          // Whether each player called Tichu
        bool grandTichuCalled[4] = {false};                     // Whether each player called Grand Tichu
        uint8_t passesInARow = 0;                               // Number of passes in a row
        std::vector<Card> cardsOnTable{};                       // Cards currently on the table
        int lastPlayerIndex{-1};                                // Index of the last player who played cards
        Tichu* pTichuGame{};                                    // Pointer to the Tichu game instance
        

        // Constructor to initialize the game state
        GameState() : currentPlayerIndex(0) 
        {
            scores.fill(0);
        }

        bool IsTerminal() const;

        // The playerPerspectiveIndex is the index of the player that we want to get the reward for.
        double GetReward(int playerPerspectiveIndex) const;

        void GetPossibleGameStates(const GameState& currentState, std::vector<GameState>& moves) const;

        int GetCurrentPlayer() const { return currentPlayerIndex; }

        bool operator==(const GameState& other) const
        {
            return playerHands == other.playerHands &&
                   teamScore == other.teamScore &&
                   opponentTeamScore == other.opponentTeamScore &&
                   scores == other.scores &&
                   currentPlayerIndex == other.currentPlayerIndex;
        }

        int GetActivePlayers() const 
        {
            int count = 0;
            for (const auto& hand : playerHands) 
            {
                if (!hand.empty()) count++;
            }
            return count;
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

        bool IsFullyExpanded();
        bool IsLeaf();
        Node* BestChild(double explorationWeight);
        Node* AddChild(const GameState& childState, int childPlayer);

        // Delete copy and move constructors and assignment operators
        Node(const Node&) = delete;
        Node& operator=(const Node&) = delete;
        Node(Node&&) = delete;
        Node& operator=(Node&&) = delete;
    };

    // Add this struct to track MCTS progress
    struct MCTSProgress {
        int currentIteration{0};
        std::vector<Card> bestMove{};
        double bestMoveScore{0.0};
        int visitCount{0};
    };

    inline std::string GetCardColourString(CardColour colour)
    {
        switch (colour)
        {
        case CC_Black:
            return "Black";
        case CC_Green:
            return "Green";
        case CC_Blue:
            return "Blue";
        case CC_Red:
            return "Red";
        case CC_Mahjong:
            return "Mahjong";
        case CC_Dragon:
            return "Dragon";
        case CC_Phoenix:
            return "Phoenix";
        case CC_Dog:
            return "Dog";
        }

        //Never reached
        return "Unknown";
    }

    // Add a progress callback type
    using ProgressCallback = std::function<void(const MCTSProgress&)>;

    // Modify the MonteCarloTreeSearch declaration
    GameState MonteCarloTreeSearch(const GameState& rootState, int iterations, 
        const ProgressCallback& progressCallback = nullptr);
}
