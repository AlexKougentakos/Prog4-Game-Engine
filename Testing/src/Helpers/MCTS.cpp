#include "MCTS.h"

#include "MCTS.h"
#include <cmath>
#include <limits>
#include <cstdlib>
#include <iostream>
#include <set>
#include <iomanip>
#include <unordered_map>
#include "Tichu.h"

namespace MCTS
{
    bool DebugLogger::s_Enabled = false;

Node::Node(const GameState& state, int player, Node* parent)
: state(state), player(player), visitCount(0), totalValue(0.0), parent(parent) {}

bool Node::IsFullyExpanded() const 
{
    std::vector<GameState> moves;
    state.GetPossibleGameStates(state, moves);

    return children.size() == moves.size();
}

bool Node::IsLeaf() const 
{
    return children.empty();
}

Node* Node::BestChild(double explorationWeight) const 
{
    Node* bestNode = nullptr;
    double bestValue = -std::numeric_limits<double>::infinity();

    for (const auto& child : children) 
    {
        double ucbValue = (child->totalValue / (child->visitCount + 1e-6)) +
                          explorationWeight * std::sqrt(std::log(visitCount + 1) / (child->visitCount + 1e-6));
        if (ucbValue > bestValue) 
        {
            bestValue = ucbValue;
            bestNode = child.get();
        }
    }
    return bestNode;
}

Node* Node::AddChild(const GameState& childState, int childPlayer) 
{
    children.push_back(std::make_unique<Node>(childState, childPlayer, this));
    return children.back().get();
}

GameState SimulateGame(GameState state) 
{
    constexpr int MAX_MOVES = 100; // Prevent infinite loops
    int moveCount = 0;
    
    while (!state.IsTerminal() && moveCount < MAX_MOVES) 
    {
        std::vector<GameState> moves;
        state.GetPossibleGameStates(state, moves);
        
        if (moves.empty()) {
            std::cout << "WARNING: No possible moves in simulation!\n";
            break;
        }

        // Debug current state
        std::cout << "Current player: " << state.currentPlayerIndex << "\n";
        std::cout << "Available moves: " << moves.size() << "\n";
        std::cout << "Cards in current player's hand: " << state.playerHands[state.currentPlayerIndex].size() << "\n";
        
        // Select and apply random move
        int moveIndex = rand() % moves.size();
        state = moves[moveIndex];
        moveCount++;
        
        if (moveCount % 10 == 0) {
            std::cout << "Simulation at move " << moveCount << "\n";
        }

        // Check if we're approaching the limit
        if (moveCount >= MAX_MOVES - 1) {
            std::cout << "WARNING: Reached maximum move limit in simulation!\n";
        }
    }
    
    std::cout << "Simulation complete after " << moveCount << " moves\n";
    if (state.IsTerminal()) {
        std::cout << "Reached terminal state naturally\n";
    }
    return state;
}

void Backpropagate(Node* node, double reward) 
{
    int depth = 0;
    while (node) 
    {
        node->visitCount++;
        node->totalValue += reward;
        reward = 1 - reward;
        node = node->parent;
        depth++;
    }
    std::cout << "Backpropagated through " << depth << " nodes\n";
}

Node* TreePolicy(Node* node) 
{
    int depth = 0;
    while (!node->state.IsTerminal()) 
    {
        std::cout << "Tree policy at depth " << depth++ << "\n";
        
        if (!node->IsFullyExpanded()) 
        {
            std::cout << "Node not fully expanded, generating possible moves...\n";
            std::vector<GameState> possibleMoves{};
            node->state.GetPossibleGameStates(node->state, possibleMoves);
            
            std::cout << "Found " << possibleMoves.size() << " possible moves\n";
            
            for (const auto& move : possibleMoves) 
            {
                bool alreadyExpanded = false;
                for (const auto& child : node->children) 
                {
                    if (child->state == move) 
                    {
                        alreadyExpanded = true;
                        break;
                    }
                }
                if (!alreadyExpanded) 
                {
                    std::cout << "Adding new child node\n";
                    return node->AddChild(move, 1 - node->player);
                }
            }
        } 
        else 
        {
            std::cout << "Node fully expanded, selecting best child...\n";
            node = node->BestChild(1.414);
            if (!node) {
                std::cout << "ERROR: BestChild returned nullptr!\n";
                break;
            }
        }
    }
    
    std::cout << "Reached terminal state or error condition\n";
    return node;
}

GameState MonteCarloTreeSearch(const GameState& rootState, int iterations) 
{
    DebugLogger::Log("\n=== Starting Monte Carlo Tree Search with %d iterations ===\n", iterations);
    Node root(rootState, rootState.GetCurrentPlayer());

    for (int i = 0; i < iterations; ++i) 
    {
        DebugLogger::Log("\nIteration %d/%d:\n", i + 1, iterations);
        DebugLogger::Log("Selection phase - Finding node to expand...\n");
        Node* node = TreePolicy(&root);
        
        DebugLogger::Log("Simulation phase - Running random playout...\n");
        GameState finalState = SimulateGame(node->state);
        
        double reward = finalState.GetReward(node->player);
        DebugLogger::Log("Backpropagation phase - Reward: %.2f\n", reward);
        
        Backpropagate(node, reward);
    }

    Node* bestNode = root.BestChild(0.0);
    if (!bestNode) {
        DebugLogger::Log("ERROR: No best child found!\n");
        return rootState;
    }
    
    DebugLogger::Log("=== MCTS Complete ===\n\n");
    return bestNode->state;
}

//--------------------------------
// GameState implementation
//--------------------------------
#pragma region GameState

void RemoveCardsFromHand(std::vector<Card>& hand, const std::vector<Card>& play)
{
    for (const Card& playedCard : play)
    {
        auto it = std::find_if(hand.begin(), hand.end(), [&playedCard](const Card& card)
        {
            return card.power == playedCard.power;
        });
        if (it != hand.end())
        {
            hand.erase(it);
        }
    }
}

void GenerateSingleCardPlays(const std::vector<Card>& hand, const Combination& currentCombination, std::vector<std::vector<Card>>& possiblePlays)
{
    for (const Card& card : hand)
    {
        if (card.power > currentCombination.power)
        {
            if (std::find(possiblePlays.begin(), possiblePlays.end(), std::vector<Card>{card}) == possiblePlays.end())
            {
                possiblePlays.emplace_back(std::vector<Card>{card});
            }
        }
    }
}

void GenerateDoubleCardPlays(const std::vector<Card>& hand, const Combination& currentCombination, std::vector<std::vector<Card>>& possiblePlays)
{
    // Group cards by power
    std::unordered_map<uint8_t, std::vector<Card>> cardGroups;
    for (const Card& card : hand)
    {
        cardGroups[card.power].push_back(card);
    }

    // Find pairs
    for (const auto& group : cardGroups)
    {
        if (group.second.size() >= 2 && group.first > currentCombination.power)
        {
            possiblePlays.push_back({ group.second[0], group.second[1] });
        }
    }
}

void GenerateTripleCardPlays(const std::vector<Card>& hand, const Combination& currentCombination, std::vector<std::vector<Card>>& possiblePlays)
{
    // Group cards by power
    std::unordered_map<uint8_t, std::vector<Card>> cardGroups;
    for (const Card& card : hand)
    {
        cardGroups[card.power].push_back(card);
    }

    // Find triples
    for (const auto& group : cardGroups)
    {
        if (group.second.size() >= 3 && group.first > currentCombination.power)
        {
            possiblePlays.push_back({ group.second[0], group.second[1], group.second[2] });
        }
    }
}

void GenerateBombPlays(const std::vector<Card>& hand, const Combination& /*currentCombination*/, std::vector<std::vector<Card>>& possiblePlays)
{

    // Group cards by power
    std::unordered_map<uint8_t, std::vector<Card>> cardGroups;
    for (const Card& card : hand)
    {
        cardGroups[card.power].push_back(card);
    }

    // Find four-of-a-kind bombs
    for (const auto& group : cardGroups)
    {
        if (group.second.size() >= 4)
        {
            possiblePlays.push_back({ group.second[0], group.second[1], group.second[2], group.second[3] });
        }
    }
}

void GenerateFullHousePlays(const std::vector<Card>& hand, const Combination& currentCombination, std::vector<std::vector<Card>>& possiblePlays)
{
    // Group cards by power
    std::unordered_map<uint8_t, std::vector<Card>> cardGroups;
    for (const Card& card : hand)
    {
        cardGroups[card.power].push_back(card);
    }

    // Find triples and pairs
    for (const auto& tripleGroup : cardGroups)
    {
        if (tripleGroup.second.size() >= 3 && tripleGroup.first > currentCombination.power)
        {
            for (const auto& pairGroup : cardGroups)
            {
                if (pairGroup.first != tripleGroup.first && pairGroup.second.size() >= 2)
                {
                    std::vector<Card> fullHouse;
                    fullHouse.insert(fullHouse.end(), tripleGroup.second.begin(), tripleGroup.second.begin() + 3);
                    fullHouse.insert(fullHouse.end(), pairGroup.second.begin(), pairGroup.second.begin() + 2);
                    possiblePlays.push_back(fullHouse);
                }
            }
        }
    }
}

void GenerateStraightPlays(const std::vector<Card>& hand, const Combination& currentCombination, std::vector<std::vector<Card>>& possiblePlays)
{
    // Collect unique card powers
    std::set<uint8_t> uniquePowers;
    std::unordered_map<uint8_t, std::vector<Card>> cardGroups;
    for (const Card& card : hand)
    {
        uniquePowers.insert(card.power);
        cardGroups[card.power].push_back(card);
    }

    // Convert to sorted vector
    std::vector<uint8_t> sortedPowers(uniquePowers.begin(), uniquePowers.end());

    // Find sequences of at least 5 consecutive powers
    for (size_t i = 0; i < sortedPowers.size(); ++i)
    {
        std::vector<Card> straight;
        uint8_t previousPower = sortedPowers[i];
        straight.push_back(cardGroups[previousPower][0]);

        for (size_t j = i + 1; j < sortedPowers.size(); ++j)
        {
            if (sortedPowers[j] == previousPower + 1)
            {
                straight.push_back(cardGroups[sortedPowers[j]][0]);
                previousPower = sortedPowers[j];

                if (straight.size() >= 5 && straight.front().power > currentCombination.power)
                {
                    possiblePlays.push_back(straight);
                }
            }
            else
            {
                break;
            }
        }
    }
}

void GenerateStepPlays(const std::vector<Card>& hand, const Combination& currentCombination, std::vector<std::vector<Card>>& possiblePlays)
{
    // Collect powers with at least two cards
    std::vector<uint8_t> pairPowers;
    std::unordered_map<uint8_t, std::vector<Card>> cardGroups;
    for (const Card& card : hand)
    {
        cardGroups[card.power].push_back(card);
    }

    for (const auto& group : cardGroups)
    {
        if (group.second.size() >= 2)
        {
            pairPowers.push_back(group.first);
        }
    }

    // Sort the pair powers
    std::sort(pairPowers.begin(), pairPowers.end());

    // Find sequences of at least two consecutive pairs
    for (size_t i = 0; i < pairPowers.size(); ++i)
    {
        std::vector<Card> step;
        uint8_t previousPower = pairPowers[i];
        step.push_back(cardGroups[previousPower][0]);
        step.push_back(cardGroups[previousPower][1]);

        for (size_t j = i + 1; j < pairPowers.size(); ++j)
        {
            if (pairPowers[j] == previousPower + 1)
            {
                step.push_back(cardGroups[pairPowers[j]][0]);
                step.push_back(cardGroups[pairPowers[j]][1]);
                previousPower = pairPowers[j];

                if (step.size() >= 4 && step.front().power > currentCombination.power)
                {
                    possiblePlays.push_back(step);
                }
            }
            else
            {
                break;
            }
        }
    }
}

void GeneratePossiblePlays(const std::vector<Card>& hand, const Combination& currentCombination, std::vector<std::vector<Card>>& possiblePlays)
{
    switch (currentCombination.combinationType)
    {
    case CombinationType::CT_Single:
        GenerateSingleCardPlays(hand, currentCombination, possiblePlays);
        break;
    case CombinationType::CT_Doubles:
        GenerateDoubleCardPlays(hand, currentCombination, possiblePlays);
        break;
    case CombinationType::CT_Triples:
        GenerateTripleCardPlays(hand, currentCombination, possiblePlays);
        break;
    case CombinationType::CT_FullHouse:
        GenerateFullHousePlays(hand, currentCombination, possiblePlays);
        break;
    case CombinationType::CT_Straight:
        GenerateStraightPlays(hand, currentCombination, possiblePlays);
        break;
    case CombinationType::CT_Steps:
        GenerateStepPlays(hand, currentCombination, possiblePlays);
        break;
    case CombinationType::CT_Invalid:
        GenerateSingleCardPlays(hand, currentCombination, possiblePlays);
        GenerateDoubleCardPlays(hand, currentCombination, possiblePlays);
        GenerateTripleCardPlays(hand, currentCombination, possiblePlays);
        GenerateFullHousePlays(hand, currentCombination, possiblePlays);
        GenerateStraightPlays(hand, currentCombination, possiblePlays);
        GenerateStepPlays(hand, currentCombination, possiblePlays);
        break;
    }
}


void GameState::GetPossibleGameStates(const GameState& currentState, std::vector<GameState>& moves) const
{
    DebugLogger::Log("\nGenerating possible moves for player %d\n", currentState.currentPlayerIndex);
    
    const std::vector<Card>& hand = currentState.playerHands[currentState.currentPlayerIndex];
    DebugLogger::Log("Player has %zu cards in hand\n", hand.size());

    std::vector<std::vector<Card>> possiblePlays{};
    GeneratePossiblePlays(hand, currentState.currentCombination, possiblePlays);
    DebugLogger::Log("Generated %zu possible plays\n", possiblePlays.size());

    // For each possible play
    for (const auto& play : possiblePlays)
    {
        // Create a new game state
        GameState newState = currentState;

        // Remove the played cards from the player's hand
        RemoveCardsFromHand(newState.playerHands[newState.currentPlayerIndex], play);

        // Update the currentPlayer
        newState.currentPlayerIndex = (newState.currentPlayerIndex + 1) % 4;

        // Add the new state to moves
        moves.push_back(newState);
    }

    // Also consider passing
    GameState passState = currentState;
    passState.currentPlayerIndex = (passState.currentPlayerIndex + 1) % 4;
    moves.push_back(passState);

    DebugLogger::Log("Total moves (including pass): %zu\n", moves.size());
}

double GameState::GetReward(int player) const
{
    // Reward is based on the team score difference
    int team1Score = scores[0] + scores[2];
    int team2Score = scores[1] + scores[3];
    if ((player % 2 == 0 && team1Score > team2Score) || (player % 2 == 1 && team2Score > team1Score)) 
    {
        return 1.0;
    } else if (team1Score == team2Score) 
    {
        return 0.5;
    }
    return 0.0;
}

std::string GetCardColourString(CardColour colour)
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

void GameState::PrintState() const
{
    //todo: implement this
}

bool GameState::IsTerminal() const 
{
    bool team1Wins = scores[0] + scores[2] >= 500;
    bool team2Wins = scores[1] + scores[3] >= 500;
    bool allPlayersOut = true;
    
    for (const auto& hand : playerHands) {
        if (!hand.empty()) {
            allPlayersOut = false;
            break;
        }
    }
    
    bool isTerminal = team1Wins || team2Wins || allPlayersOut;

    DebugLogger::Log("IsTerminal check:\n"
                    "Team 1 score: %d\n"
                    "Team 2 score: %d\n"
                    "All players out: %s\n"
                    "State is terminal: %s\n",
                    scores[0] + scores[2],
                    scores[1] + scores[3],
                    allPlayersOut ? "true" : "false",
                    isTerminal ? "true" : "false");
    
    return isTerminal;
}
#pragma endregion
}