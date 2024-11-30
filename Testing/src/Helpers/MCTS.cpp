#include "MCTS.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <cstdlib>
#include <set>
#include <unordered_map>
#include <atomic>
#include <thread>

namespace std
{
    template <>
    struct hash<Card>
    {
        std::size_t operator()(const Card& card) const noexcept
        {
            std::size_t h1 = std::hash<uint8_t>{}(card.colour);
            std::size_t h2 = std::hash<uint8_t>{}(card.power);
            return h1 ^ (h2 << 1); // Combine hashes
        }
    };
}

namespace std
{
    template <>
    struct hash<Combination>
    {
        std::size_t operator()(const Combination& combo) const noexcept
        {
            std::size_t h1 = std::hash<int>{}(static_cast<int>(combo.combinationType));
            std::size_t h2 = std::hash<uint8_t>{}(combo.power);
            return h1 ^ (h2 << 1); // Combine hashes
        }
    };
}

namespace std
{
    template <>
    struct hash<MCTS::GameState>
    {
        std::size_t operator()(const MCTS::GameState& state) const noexcept
        {
            std::size_t seed = 0;
            
            // Hash playerHands
            for (const auto& hand : state.playerHands)
            {
                for (const auto& card : hand)
                {
                    seed ^= std::hash<Card>{}(card) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
                }
            }
            
            // Hash scores
            for (const auto& score : state.scores)
            {
                seed ^= std::hash<int8_t>{}(score) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
            
            // Hash currentPlayerIndex
            seed ^= std::hash<uint8_t>{}(state.currentPlayerIndex) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            
            // Hash currentCombination
            seed ^= std::hash<Combination>{}(state.currentCombination) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            
            // Hash passesInARow
            seed ^= std::hash<uint8_t>{}(state.passesInARow) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            
            // Hash lastPlayerIndex
            seed ^= std::hash<int>{}(state.lastPlayerIndex) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            
            // Optionally hash tichuCalled and grandTichuCalled arrays
            for (bool called : state.tichuCalled)
            {
                seed ^= std::hash<bool>{}(called) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
            for (bool called : state.grandTichuCalled)
            {
                seed ^= std::hash<bool>{}(called) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
            
            // Hash cardsOnTable
            for (const auto& card : state.cardsOnTable)
            {
                seed ^= std::hash<Card>{}(card) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
            
            // Note: We are not hashing pointers like pTichuGame.
            // If needed, you can include teamScore and opponentTeamScore.

            return seed;
        }
    };
}

namespace MCTS
{
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

    bool Node::IsFullyExpanded() 
    {
        std::vector<GameState> moves;
        state.GetPossibleGameStates(state, moves);

        return children.size() == moves.size();
    }

    bool Node::IsLeaf() 
    {
        return children.empty();
    }

    Node* Node::BestChild(double explorationWeight) 
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
        while (!state.IsTerminal()) 
        {
            std::vector<GameState> moves;
            state.GetPossibleGameStates(state, moves);

            if (moves.empty()) 
            {
                break;
            }

            // Select and apply random move
            int moveIndex = rand() % moves.size();
            state = moves[moveIndex];
        }
        return state;
    }

    void Backpropagate(Node* node, double reward) 
    {
        while (node) 
        {
            node->visitCount++;
            node->totalValue += reward;
            reward = 1 - reward;
            node = node->parent;
        }
    }

    Node* TreePolicy(Node* node) 
    {
        while (!node->state.IsTerminal()) 
        {
            if (!node->IsFullyExpanded()) 
            {
                std::vector<GameState> possibleMoves{};
                node->state.GetPossibleGameStates(node->state, possibleMoves);

                if (possibleMoves.empty()) {
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
                        return node->AddChild(move, (node->player + 1) % 4); // Use modulo 4 for player rotation
                    }
                }
            }

            Node* nextNode = node->BestChild(2.1);
            if (!nextNode) {
                return node;
            }
            node = nextNode;
        }
        return node;
    }

    

    GameState MonteCarloTreeSearch(const GameState& rootState, int iterations, 
        [[maybe_unused]]const ProgressCallback& progressCallback)
    {
        int numThreads = std::min(static_cast<int>(std::thread::hardware_concurrency()), iterations);
        if (numThreads == 0) numThreads = 4; // Default to 4 threads if unable to detect

        //numThreads = 1;
        
        int iterationsPerThread = iterations / numThreads;
        int remainingIterations = iterations % numThreads;

        std::vector<std::thread> threads;
        std::vector<std::unique_ptr<Node>> rootNodes(numThreads);

        // For progress tracking
        std::atomic<int> totalIterations{0};

        for (int t = 0; t < numThreads; ++t)
        {
            int iterationsForThisThread = iterationsPerThread + (t < remainingIterations ? 1 : 0);

            rootNodes[t] = std::make_unique<Node>(rootState, rootState.GetCurrentPlayer());

            threads.emplace_back([&, t, iterationsForThisThread]()
            {
                Node* root = rootNodes[t].get();
                for (int i = 0; i < iterationsForThisThread; ++i)
                {
                    Node* node = TreePolicy(root);
                    GameState finalState = SimulateGame(node->state);
                    double reward = finalState.GetReward(rootState.GetCurrentPlayer());
                    Backpropagate(node, reward);

                    ++totalIterations;
                }
            });
        }

        // Join threads
        for (auto& thread : threads)
        {
            thread.join();
        }

        // Aggregate results
        std::unordered_map<GameState, int> childVisitCounts;
        std::unordered_map<GameState, double> childTotalValues;

        for (const auto& rootPtr : rootNodes)
        {
            Node* root = rootPtr.get();
            for (const auto& child : root->children)
            {
                childVisitCounts[child->state] += child->visitCount;
                childTotalValues[child->state] += child->totalValue;
            }
        }

        // Select the best move
        GameState bestState;
        int maxVisits = -1;
        for (const auto& [state, visits] : childVisitCounts)
        {
            if (visits > maxVisits)
            {
                maxVisits = visits;
                bestState = state;
            }
        }

        return bestState;
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
    case CombinationType::CT_Dogs:
        // Dogs can only be played when no other cards are on the table
        if (currentCombination.combinationType == CombinationType::CT_Invalid)
        {
            for (const auto& card : hand)
            {
                if (card.colour == CC_Dog)
                {
                    possiblePlays.push_back({card});
                }
            }
        }
        break;
    case CombinationType::CT_SinglePhoenix:
        // Phoenix can be played as any single card
        for (const auto& card : hand)
        {
            if (card.colour == CC_Phoenix)
            {
                possiblePlays.push_back({card});
            }
        }
        break;
    case CombinationType::CT_FourOfAKindBomb:
        GenerateBombPlays(hand, currentCombination, possiblePlays);
        break;
    case CombinationType::CT_StraightBomb:
        // Handle straight bombs if needed
        break;
    }
}

void GameState::GetPossibleGameStates(const GameState& currentState, std::vector<GameState>& moves) const
{
    
    const std::vector<Card>& hand = currentState.playerHands[currentState.currentPlayerIndex];

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
            newState.scores[newState.lastPlayerIndex] += static_cast<int8_t>(points);

            // Clear the table and reset combination
            newState.cardsOnTable.clear();
            newState.currentCombination = Combination{}; // Reset to invalid combination
            newState.passesInARow = 0;

            // Set current player to the last player who made a play
            newState.currentPlayerIndex = static_cast<int8_t>(newState.lastPlayerIndex);
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
}

double GameState::GetReward(int playerPerspectiveIndex) const
{
    int playersOut{};
    for (int i = 0; i < 4; i++)
    {
        if (playerHands[i].empty())
        {
            playersOut++;
        }
    }

    //We don't need to check for indecies, since this will always be called when the state is terminal
    //So if it's terminal, and we know there are two players out, we know it's a 1-2 finish
    //We just need to know for which team
    bool ourTeam1_2 = false;
    if (playersOut == 2)
    {
        if ((playerPerspectiveIndex == 0 || playerPerspectiveIndex == 2) && (lastPlayerIndex == 0 || lastPlayerIndex == 2))
            ourTeam1_2 = true;
        if ((playerPerspectiveIndex == 1 || playerPerspectiveIndex == 3) && (lastPlayerIndex == 1 || lastPlayerIndex == 3))
            ourTeam1_2 = true;

        if (ourTeam1_2)
        {
            return 2.0;
        }
        else
        {
            return -2.0;
        }
    }

    //If it's not a 1-2 finish, we need to calculate the reward based on the score difference between teams
    const float ourTeamScore = static_cast<float>(scores[playerPerspectiveIndex] + scores[(playerPerspectiveIndex + 2) % 4]);
    const float otherTeamScore = static_cast<float>(scores[(playerPerspectiveIndex + 1) % 4] + scores[(playerPerspectiveIndex + 3) % 4]);

    //We divide by 100.0f to normalize the score difference
    return (ourTeamScore - otherTeamScore) / 100.0f;
}

bool GameState::IsTerminal() const 
{
    // Count how many players are out
    int playersOut = 0;
    int indexOfFirstPlayerOut = -1;

    for (int i = 0; i < 4; i++) 
    {
        if (playerHands[i].empty()) 
        {
            bool is1_2Finish = false;
            playersOut++;
            if (indexOfFirstPlayerOut == -1)
                indexOfFirstPlayerOut = i;

            //Check if it's a 1-2 finish, meaning two players from the same team are out one after the other
            if (playersOut == 2)
            {
                if ((indexOfFirstPlayerOut == 0 || indexOfFirstPlayerOut == 2) && (i == 0 || i == 2))
                    is1_2Finish = true;
                if ((indexOfFirstPlayerOut == 1 || indexOfFirstPlayerOut == 3) && (i == 1 || i == 3))
                    is1_2Finish = true;

                if (is1_2Finish) return true;
            }
        }
    }
    
    const bool isTerminal = playersOut >= 3;
        
    return isTerminal;
}

#pragma endregion
}