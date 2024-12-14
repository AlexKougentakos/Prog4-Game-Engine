#include "MCTS.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <set>
#include <unordered_map>
#include <random>
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
    int CountPointsOfCards(const std::vector<Card>& cards)
    {
        int points{};
        for (const auto& card : cards)
        {
            if (card.power == 5) points += 5;
            if (card.power == 10 || card.power == 13) points += 10;
            if (card.colour == CC_Dragon) points += 25;
            if (card.colour == CC_Phoenix) points -= 25;
        }

        return points;
    }
    
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
        // Initialize a random number generator with a non-deterministic seed
        std::random_device rd{};
        std::mt19937 gen(rd());

        while (!state.IsTerminal()) 
        {
            std::vector<GameState> moves;
            state.GetPossibleGameStates(state, moves);

            if (moves.empty()) 
            {
                break;
            }

            // Create a distribution to select a valid index
            std::uniform_int_distribution<> dis(0, static_cast<int>(moves.size()) - 1);
            const int moveIndex = dis(gen); // Generate a random index

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

                if (possibleMoves.empty())
                {
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

        [[maybe_unused]] int i{};
        return node;
    }

    

    GameState MonteCarloTreeSearch(const GameState& rootState, int iterations)
    {
        int numThreads = std::min(static_cast<int>(std::thread::hardware_concurrency()), iterations);
        if (numThreads == 0) numThreads = 4; // Default to 4 threads if unable to detect

#ifdef _DEBUG
        numThreads = 1;
#endif
        
        const int iterationsPerThread = iterations / numThreads;
        const int remainingIterations = iterations % numThreads;

        std::vector<std::thread> threads{};
        std::vector<std::unique_ptr<Node>> rootNodes(numThreads);

        for (int threadIdx = 0; threadIdx < numThreads; ++threadIdx)
        {
            //Assign the remaining iterations to the first threads
            //This works because remainingIterations is the remainder of the division, so it will always be smaller than the
            //number of threads
            const int iterationsForThisThread = iterationsPerThread + (threadIdx < remainingIterations ? 1 : 0);

            //Create a new root node for each thread
            rootNodes[threadIdx] = std::make_unique<Node>(rootState, rootState.GetCurrentPlayer());

            threads.emplace_back([&, threadIdx, iterationsForThisThread]()
            {
                Node* root = rootNodes[threadIdx].get();
                for (int i = 0; i < iterationsForThisThread; ++i)
                {
                    //Time this iteration
                    Node* node = TreePolicy(root);
                    GameState finalState = SimulateGame(node->state);
                    finalState.DistributePoints();
                    const double reward = finalState.GetReward(rootState.GetCurrentPlayer());
                    Backpropagate(node, reward);
                }
            });
        }

        // Join threads
        for (auto& thread : threads)
        {
            thread.join();
        }

        // Aggregate results
        std::unordered_map<GameState, int> childVisitCounts{};
        std::unordered_map<GameState, double> childTotalValues{};

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


void GenerateFilledStraights(const std::vector<Card>& hand, const Combination& currentCombination, std::vector<std::vector<Card>>& possiblePlays)
{
    // 1) Separate the Phoenix card and collect normal cards
    Card phoenixCard;           
    bool hasPhoenix = false;

    std::set<uint8_t> uniquePowers;
    std::unordered_map<uint8_t, std::vector<Card>> cardGroups;

    for (const Card& card : hand)
    {
        if (card.colour == CC_Phoenix && card.power == 0)
        {
            phoenixCard = card;
            hasPhoenix = true;
        }
        else
        {
            uniquePowers.insert(card.power);
            cardGroups[card.power].push_back(card);
        }
    }

    // Convert to sorted vector of unique powers (excluding phoenix = 0)
    std::vector<uint8_t> sortedPowers(uniquePowers.begin(), uniquePowers.end());
    // sortedPowers is now in ascending order

    // 2) Find sequences of at least 5 consecutive powers, accounting for Phoenix usage
    for (size_t i = 0; i < sortedPowers.size(); ++i)
    {
        std::vector<Card> straight;
        straight.push_back(cardGroups[sortedPowers[i]][0]);  // start with the first card for that power
        uint8_t previousPower = sortedPowers[i];

        bool usedPhoenix = false;

        for (size_t j = i + 1; j < sortedPowers.size(); ++j)
        {
            uint8_t currentPower = sortedPowers[j];

            // Case 1: Perfectly consecutive
            if (currentPower == previousPower + 1)
            {
                straight.push_back(cardGroups[currentPower][0]);
                previousPower = currentPower;
            }
            // Case 2: Use Phoenix to bridge exactly a gap of 1 power (e.g., from 5 to 7)
            else if (hasPhoenix && !usedPhoenix && currentPower == previousPower + 2)
            {
                // Insert Phoenix at the front (user requirement)
                // *No* power modification on the Phoenix card
                straight.insert(straight.begin(), phoenixCard);

                // Then push the actual card for currentPower
                straight.push_back(cardGroups[currentPower][0]);
                previousPower = currentPower;
                usedPhoenix = true;
            }
            else
            {
                // If gap > 1 (bigger than a single skip) or Phoenix already used, we can’t continue this sequence
                break;
            }

            // Once we have a new card in straight, check if >= 5
            if (straight.size() >= 5)
            {
                const int powerToCompare = usedPhoenix ? straight[1].power : straight.front().power;
                if (powerToCompare > currentCombination.power)
                {
                    possiblePlays.push_back(straight);
                }
            }
        }
    }
}


void GeneratePureStraights(const std::vector<Card>& hand, const Combination& currentCombination, std::vector<std::vector<Card>>& possiblePlays)
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

void GenerateStraightPlays(const std::vector<Card>& hand, const Combination& currentCombination, std::vector<std::vector<Card>>& possiblePlays)
{
    // Identify the Phoenix card if it exists
    bool hasPhoenix = false;
    Card phoenixCard{};
    for (const Card& card : hand)
    {
        if (card.colour == CC_Phoenix)
        {
            hasPhoenix = true;
            phoenixCard = card; // keep a copy of the phoenix
            break;
        }
    }

    std::vector<std::vector<Card>> pureStraights{};
    GeneratePureStraights(hand, currentCombination, pureStraights);
        
    if (hasPhoenix)
    {
        GenerateFilledStraights(hand, currentCombination, possiblePlays);

        //Add the phoenix to the beginning of each straight
        for (const std::vector<Card>& straight : pureStraights)
        {
            //We can't go lower than a Mahjong
            if (straight[0].power == 1 ) continue;
            
            std::vector<Card> moveWithPhoenix = straight;
            moveWithPhoenix.insert(moveWithPhoenix.begin(), phoenixCard);
            possiblePlays.push_back(moveWithPhoenix);
        }
    }
    else
    {
        possiblePlays.insert(possiblePlays.end(), pureStraights.begin(), pureStraights.end());
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

    // If the player's hand is empty, they are out.
    // If firstPlayerOutIndex is not yet set, set it now.
    // (This might indicate the player went out in a previous turn and we are just now processing it.)
    if (hand.empty())
    {
        GameState newState = currentState;
        if (newState.firstPlayerOutIndex == -1)
        {
            newState.firstPlayerOutIndex = newState.currentPlayerIndex;
        }

        // Player has no cards left; advance to the next player who still has cards
        newState.currentPlayerIndex = (newState.currentPlayerIndex + 1) % 4;
        while (newState.playerHands[newState.currentPlayerIndex].empty())
        {
            newState.currentPlayerIndex = (newState.currentPlayerIndex + 1) % 4;
        }
        moves.push_back(newState);
        return;
    }

    // Generate possible plays based on the current combination
    std::vector<std::vector<Card>> possiblePlays{};
    //GeneratePossiblePlays(hand, currentState.currentCombination, possiblePlays);
    GenerateStraightPlays(hand, currentState.currentCombination, possiblePlays);
    

    // For each possible play
    for (auto& play : possiblePlays)
    {
        // Create a new game state
        GameState newState = currentState;

        // Remove the played cards from the player's hand
        RemoveCardsFromHand(newState.playerHands[newState.currentPlayerIndex], play);

        // If this player just went out (hand is now empty) and no one was out before, mark them as first out
        if (newState.playerHands[newState.currentPlayerIndex].empty() && newState.firstPlayerOutIndex == -1)
        {
            newState.firstPlayerOutIndex = newState.currentPlayerIndex;
        }

        // Update the current combination
        newState.currentCombination = newState.pTichuGame->CreateCombination(play);

        // Add cards to the table
        newState.cardsOnTable.insert(newState.cardsOnTable.end(), play.begin(), play.end());
        newState.lastPlayerIndex = newState.currentPlayerIndex;
        newState.passesInARow = 0;

        // Move to the next player who still has cards
        newState.currentPlayerIndex = (newState.currentPlayerIndex + 1) % 4;
        while (newState.playerHands[newState.currentPlayerIndex].empty())
        {
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

        // If the player has no valid plays
        if (possiblePlays.empty())
        {
            // Handle the scenario where player must play their lowest card
            GameState newState = currentState;
            std::vector<Card> lowestCard = { *std::min_element(hand.begin(), hand.end(), [](const Card& a, const Card& b) {
                return a.power < b.power;
            }) };

            RemoveCardsFromHand(newState.playerHands[newState.currentPlayerIndex], lowestCard);

            // If this player just went out
            if (newState.playerHands[newState.currentPlayerIndex].empty() && newState.firstPlayerOutIndex == -1)
            {
                newState.firstPlayerOutIndex = newState.currentPlayerIndex;
            }

            newState.currentCombination = newState.pTichuGame->CreateCombination(lowestCard);
            newState.cardsOnTable.insert(newState.cardsOnTable.end(), lowestCard.begin(), lowestCard.end());
            newState.lastPlayerIndex = newState.currentPlayerIndex;
            newState.passesInARow = 0;

            // Move to next player who still has cards
            newState.currentPlayerIndex = (newState.currentPlayerIndex + 1) % 4;
            while (newState.playerHands[newState.currentPlayerIndex].empty())
            {
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
        while (passState.playerHands[passState.currentPlayerIndex].empty())
        {
            passState.currentPlayerIndex = (passState.currentPlayerIndex + 1) % 4;
        }

        // Check if the round should end
        int activePlayers = GetActivePlayers();
        if (passState.passesInARow >= activePlayers - 1)
        {
            // The last player who played wins the cards on the table
            GameState newState = passState;

            // Calculate points from cards on the table
            const int points = CountPointsOfCards(newState.cardsOnTable);

            // Add points according to the dragon card rule
            if (cardsOnTable[cardsOnTable.size() - 1].colour == CC_Dragon)
            {
                int current = newState.lastPlayerIndex;
                int opponent1, opponent2;
                if (current == 0 || current == 2)
                {
                    opponent1 = 1;
                    opponent2 = 3;
                }
                else
                {
                    opponent1 = 0;
                    opponent2 = 2;
                }

                bool opp1InPlay = !newState.playerHands[opponent1].empty();
                bool opp2InPlay = !newState.playerHands[opponent2].empty();

                int chosenOpponent;
                if (opp1InPlay) chosenOpponent = opponent1;
                else if (opp2InPlay) chosenOpponent = opponent2;
                else chosenOpponent = opponent1; // Fallback

                newState.scores[chosenOpponent] += static_cast<int8_t>(points);
            }
            else
            {
                newState.scores[newState.lastPlayerIndex] += static_cast<int8_t>(points);
            }

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

            moves.push_back(newState);
        }
        else
        {
            // Normal pass, add to possible moves
            moves.push_back(passState);
        }
    }
}

double GameState::GetReward(int playerPerspectiveIndex)
{
    int playersOut{};
    int indexOfPlayerNotOut{};
    for (int i = 0; i < 4; i++)
    {
        if (playerHands[i].empty())
        {
            playersOut++;
        }
        else
        {
            indexOfPlayerNotOut = i;
        }
    }

    //We don't need to check for indices, since this will always be called when the state is terminal
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

void GameState::DistributePoints()
{
        int playersOut{};
        int indexOfPlayerNotOut{};
        for (int i = 0; i < 4; i++)
        {
            if (playerHands[i].empty())
            {
                playersOut++;
            }
            else
            {
                indexOfPlayerNotOut = i;
            }
        }

        //The points on the hand of the player who didn't go out go to the other team
        const int scoreInFinalHand = CountPointsOfCards(playerHands[indexOfPlayerNotOut]);
        scores[(indexOfPlayerNotOut + 1) % 4] += static_cast<int8_t>(scoreInFinalHand);

        //The points on the table go to the player who threw the last card
        const int pointsOnTable = CountPointsOfCards(cardsOnTable);

        //Exception for the dragon card. This would be given to the player who is not out if it's the last card
        if (cardsOnTable[cardsOnTable.size() - 1].colour == CC_Dragon)
            scores[indexOfPlayerNotOut] += static_cast<int8_t>(pointsOnTable);
        else scores[lastPlayerIndex] += static_cast<int8_t>(pointsOnTable);
        
        //The points collected by the player who didn't go out, go to the team of the player who went out first
        //This has to be done after the points on the table are distributed, in case of the dragon being the last card
        scores[firstPlayerOutIndex] += scores[indexOfPlayerNotOut];
        scores[indexOfPlayerNotOut] = 0;
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