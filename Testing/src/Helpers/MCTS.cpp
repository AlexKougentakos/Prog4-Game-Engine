#include "MCTS.h"
#include <cmath>
#include <limits>
#include <cstdlib>
#include <iostream>
#include <set>
#include <iomanip>
#include <unordered_map>
#include "Tichu.h"

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


namespace MCTS
{
    bool DebugLogger::s_Enabled = false;


std::string CardToString(const Card& card) 
{
    std::string result = GetCardColourString(card.colour);
    if (card.colour <= CC_Red) 
    { // Normal cards
            result += "-" + std::to_string(static_cast<int>(card.power));
        }
        return result;
    }

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
    constexpr int MAX_MOVES = 100;
    int moveCount = 0;
    
    std::cout << "Simulating game started\n";
    while (!state.IsTerminal() && moveCount < MAX_MOVES) 
    {
        std::vector<GameState> moves;
        state.GetPossibleGameStates(state, moves);
        
        if (moves.empty()) 
        {
            DebugLogger::Log("WARNING: No possible moves in simulation!\n");
            break;
        }

        // Debug current state
        DebugLogger::Log("Current player: %d\n", state.currentPlayerIndex);
        DebugLogger::Log("Available moves: %zu\n", moves.size());
        
        // Log current player's hand
        DebugLogger::Log("Current player's hand: ");
        for (const auto& card : state.playerHands[state.currentPlayerIndex]) 
        {
            DebugLogger::Log("%s, ", CardToString(card).c_str());
        }
        DebugLogger::Log("\n");
        
        // Select and apply random move
        int moveIndex = rand() % moves.size();
        
        // Log the selected move
        DebugLogger::Log("Selected move %d: ", moveIndex);
        const auto& selectedMove = moves[moveIndex];
        // Find the cards that were played by comparing hands
        std::vector<Card> playedCards;
        for (const auto& card : state.playerHands[state.currentPlayerIndex]) {
            if (std::find_if(selectedMove.playerHands[state.currentPlayerIndex].begin(), 
                           selectedMove.playerHands[state.currentPlayerIndex].end(),
                           [&card](const Card& c) { return c.power == card.power && c.colour == card.colour; }) 
                == selectedMove.playerHands[state.currentPlayerIndex].end()) {
                playedCards.push_back(card);
            }
        }
        if (playedCards.empty()) {
            DebugLogger::Log("PASS\n");
        } else {
            for (const auto& card : playedCards) {
                DebugLogger::Log("%s, ", CardToString(card).c_str());
            }
            DebugLogger::Log("\n");
        }
        
        state = moves[moveIndex];
        moveCount++;
        
        if (moveCount % 10 == 0) {
            DebugLogger::Log("Simulation at move %d\n", moveCount);
        }

        if (moveCount >= MAX_MOVES - 1) {
            DebugLogger::Log("WARNING: Reached maximum move limit in simulation!\n");
        }
    }

    std::cout << "Simulating game finished\n";
    
    DebugLogger::Log("Simulation complete after %d moves\n", moveCount);
    if (state.IsTerminal()) {
        DebugLogger::Log("Reached terminal state naturally\n");
    }
    return state;
}

void Backpropagate(Node* node, double reward) 
{
    std::cout << "Backpropagate started\n";
    int depth = 0;
    while (node) 
    {
        std::cout << "Backpropagate at depth " << depth << "\n";

        node->visitCount++;
        node->totalValue += reward;
        reward = 1 - reward;
        node = node->parent;
        depth++;
    }
    std::cout << "Backpropagate finished\n";

    DebugLogger::Log("Backpropagated through %d nodes\n", depth);
}

Node* TreePolicy(Node* node) 
{
    int depth = 0;
    /* constexpr int MAX_DEPTH = 100; */
    
    while (!node->state.IsTerminal() /* && depth < MAX_DEPTH */) 
    {
        DebugLogger::Log("Tree policy at depth %d\n", depth++);
        
        if (!node->IsFullyExpanded()) 
        {
            std::vector<GameState> possibleMoves{};
            node->state.GetPossibleGameStates(node->state, possibleMoves);
            
            if (possibleMoves.empty()) {
                DebugLogger::Log("No possible moves, treating as terminal\n");
                return node;
            }
            
            // Check for already expanded states
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
                    DebugLogger::Log("Adding new child node\n");
                    return node->AddChild(move, (node->player + 1) % 4);
                }
            }
        }
        
        Node* nextNode = node->BestChild(1.414);
        if (!nextNode) {
            DebugLogger::Log("No best child found, treating as terminal\n");
            return node;
        }
        node = nextNode;
    }
    
    DebugLogger::Log("Reached terminal state or max depth\n");
    return node;
}

GameState MonteCarloTreeSearch(const GameState& rootState, int iterations) 
{
    DebugLogger::Log("\n=== Starting Monte Carlo Tree Search with %d iterations ===\n", iterations);
    Node root(rootState, rootState.currentPlayerIndex);

    for (int i = 0; i < iterations; ++i) 
    {
        DebugLogger::Log("\nIteration %d/%d:\n", i + 1, iterations);
        
        Node* node = TreePolicy(&root);
        if (!node) {
            DebugLogger::Log("ERROR: TreePolicy returned nullptr!\n");
            continue;
        }
        
        GameState finalState = SimulateGame(node->state);
        double reward = finalState.GetReward(node->player);
        
        Backpropagate(node, reward);
    }

    Node* bestNode = root.BestChild(0.0);
    if (!bestNode) {
        DebugLogger::Log("ERROR: No best child found!\n");
        return rootState;
    }
    
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
    DebugLogger::Log("Player's hand: ");
    for (const auto& card : hand) {
        DebugLogger::Log("%s, ", CardToString(card).c_str());
    }
    DebugLogger::Log("\n");

    // Check if the player's hand is empty; if so, they cannot play and the game should handle this case appropriately
    if (hand.empty()) {
        // Player has no cards left; advance to the next player
        GameState newState = currentState;
        newState.currentPlayerIndex = (newState.currentPlayerIndex + 1) % 4;
        while (newState.playerHands[newState.currentPlayerIndex].empty()) {
            newState.currentPlayerIndex = (newState.currentPlayerIndex + 1) % 4;
        }
        moves.push_back(newState);
        return;
    }

    // Generate possible plays based on the current combination
    std::vector<std::vector<Card>> possiblePlays{};
    GeneratePossiblePlays(hand, currentState.currentCombination, possiblePlays);
    
    DebugLogger::Log("Generated %zu possible plays:\n", possiblePlays.size());
    for (size_t i = 0; i < possiblePlays.size(); ++i) {
        DebugLogger::Log("Play %zu: ", i);
        for (const auto& card : possiblePlays[i]) {
            DebugLogger::Log("%s, ", CardToString(card).c_str());
        }
        DebugLogger::Log("\n");
    }

    // For each possible play
    for (auto& play : possiblePlays)
    {
        // Create a new game state
        GameState newState = currentState;

        // Remove the played cards from the player's hand
        RemoveCardsFromHand(newState.playerHands[newState.currentPlayerIndex], play);

        // Update the current combination
        newState.currentCombination = newState.pTichuGame->CreateCombination(play);

        // Add cards to the table
        newState.cardsOnTable.insert(newState.cardsOnTable.end(), play.begin(), play.end());
        newState.lastPlayerIndex = newState.currentPlayerIndex;
        newState.passesInARow = 0;

        // Move to next player
        newState.currentPlayerIndex = (newState.currentPlayerIndex + 1) % 4;
        while (newState.playerHands[newState.currentPlayerIndex].empty()) {
            newState.currentPlayerIndex = (newState.currentPlayerIndex + 1) % 4;
        }

        // Add the new state to moves
        moves.push_back(newState);
    }

    // Determine if passing is allowed
    bool canPass = true;
    if (currentState.currentCombination.combinationType == CombinationType::CT_Invalid)
    {
        // Table is empty; player cannot pass
        canPass = false;

        // If the player has no valid plays, we need to handle this
        if (possiblePlays.empty())
        {
            // Player cannot make a move; in Tichu, this situation shouldn't occur as players should always be able to play something when leading
            // But in case it happens (e.g., due to game logic or special cards), we need to handle it appropriately
            DebugLogger::Log("Player %d cannot make any valid plays when required to lead.\n", currentState.currentPlayerIndex);
            // Handle according to your game rules; perhaps the player must play their lowest card
            // For this example, we'll assume the player must play their lowest card
            GameState newState = currentState;
            std::vector<Card> lowestCard = { *std::min_element(hand.begin(), hand.end(), [](const Card& a, const Card& b) {
                return a.power < b.power;
            }) };
            RemoveCardsFromHand(newState.playerHands[newState.currentPlayerIndex], lowestCard);
            newState.currentCombination = newState.pTichuGame->CreateCombination(lowestCard);
            newState.cardsOnTable.insert(newState.cardsOnTable.end(), lowestCard.begin(), lowestCard.end());
            newState.lastPlayerIndex = newState.currentPlayerIndex;
            newState.passesInARow = 0;

            // Move to next player
            newState.currentPlayerIndex = (newState.currentPlayerIndex + 1) % 4;
            while (newState.playerHands[newState.currentPlayerIndex].empty()) {
                newState.currentPlayerIndex = (newState.currentPlayerIndex + 1) % 4;
            }

            moves.push_back(newState);
            return;
        }
    }

    if (canPass)
    {
        // Consider passing
        GameState passState = currentState;
        passState.passesInARow++;
        passState.currentPlayerIndex = (passState.currentPlayerIndex + 1) % 4;
        while (passState.playerHands[passState.currentPlayerIndex].empty()) {
            passState.currentPlayerIndex = (passState.currentPlayerIndex + 1) % 4;
        }

        // Check if the round should end
        int activePlayers = GetActivePlayers();
        if (passState.passesInARow >= activePlayers - 1)
        {
            // The last player who played wins the cards on the table
            GameState newState = passState;

            // Calculate points from cards on the table
            int points = 0;
            for (const auto& card : newState.cardsOnTable)
            {
                if (card.power == 5) points += 5;
                if (card.power == 10 || card.power == 13) points += 10;
                if (card.colour == CC_Dragon) points += 25;
                if (card.colour == CC_Phoenix) points -= 25; // Adjust if Phoenix has negative points
            }

            // Add points to the last player who played
            newState.scores[newState.lastPlayerIndex] += points;

            // Clear the table and reset combination
            newState.cardsOnTable.clear();
            newState.currentCombination = Combination{}; // Reset to invalid combination
            newState.passesInARow = 0;

            // Set current player to the last player who made a play
            newState.currentPlayerIndex = newState.lastPlayerIndex;
            while (newState.playerHands[newState.currentPlayerIndex].empty()) 
            {
                newState.currentPlayerIndex = (newState.currentPlayerIndex + 1) % 4;
            }

            // Add the new state to moves
            moves.push_back(newState);
        }
        else
        {
            // Normal pass, add to possible moves
            moves.push_back(passState);
        }
    }

    DebugLogger::Log("Total moves (including pass if allowed): %zu\n", moves.size());
}



double GameState::GetReward(int player) const
{
    //todo: rework this
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

bool GameState::IsTerminal() const 
{
    // Count how many players are out
    int playersOut = 0;
    int indexOfFirstPlayerOut = -1;
    std::vector<int> outPlayerIndices;

    for (int i = 0; i < 4; i++) 
    {
        if (playerHands[i].empty()) 
        {
            playersOut++;
            if (indexOfFirstPlayerOut == -1)
                indexOfFirstPlayerOut = i;
            outPlayerIndices.push_back(i);
        }
    }

    // Check for 1-2 finish (two players from same team out)
    if (playersOut == 2) 
    {
        bool isTeam1Out = (outPlayerIndices[0] == 0 && outPlayerIndices[1] == 2) || 
                         (outPlayerIndices[0] == 2 && outPlayerIndices[1] == 0);
        bool isTeam2Out = (outPlayerIndices[0] == 1 && outPlayerIndices[1] == 3) || 
                         (outPlayerIndices[0] == 3 && outPlayerIndices[1] == 1);
        
        if (isTeam1Out || isTeam2Out) {
            return true;
        }
    }

    // Game is terminal if 3 or more players are out
    return playersOut >= 3;
}

#pragma endregion
}