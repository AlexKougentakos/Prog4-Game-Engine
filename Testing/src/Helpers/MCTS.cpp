#include "MCTS.h"

#include <algorithm>
#include <cmath>
#include <imgui_internal.h>
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
    void RemoveCardsFromHand(std::vector<Card>& hand, const std::vector<Card>& play)
    {
        for (const Card& playedCard : play)
        {
            auto it = std::find_if(hand.begin(), hand.end(), [&playedCard](const Card& card)
            {
                //Return the phoenix by colour because the power changes
                if (card.colour == CC_Phoenix && playedCard.colour == CC_Phoenix)
                {
                    return true;
                }
                return card.power == playedCard.power && card.colour == playedCard.colour;
            });
            if (it != hand.end())
            {
                hand.erase(it);
            }
        }
    }
    
    int CountPointsOfCards(const std::vector<Card>& cards)
    {
        int points{};
        for (const auto& card : cards)
        {
            if (card.colour == CC_Phoenix)
            {
                points -= 25;
                continue; //The phoenix can take the power of other cards, so we skip the power check
            }
            if (card.power == 5) points += 5;
            if (card.power == 10 || card.power == 13) points += 10;
            if (card.colour == CC_Dragon) points += 25;
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

    Node::Node(const GameState& state, int seatOfPerspective, Node* parent)
        : state(state)
        , player(seatOfPerspective)
        , visitCount(0)
        , totalValue(0.0)
        , parent(parent)
    {}

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

    Node* Node::AddChild(const GameState& childState)
    {
        // Always pass 'this->player' so perspective never changes
        children.push_back(std::make_unique<Node>(childState, this->player, this));
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

            // if (moves.empty()) 
            // {
            //     break;
            // }

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
            //reward = 1 - reward; remove the flipping
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
                        //return node->AddChild(move, (node->player + 1) % 4); // Use modulo 4 for player rotation
                        return node->AddChild(move);

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

bool TestCombinationValidity(std::vector<Card>& combination)
{
    auto test = Tichu::CreateCombination_(combination);
    if (test.combinationType == CombinationType::CT_Invalid && test.numberOfCards != 0)
    {
        return false;
    }

    return true;
}
    
void GenerateSingleCardPlays(const std::vector<Card>& hand, const Combination& currentCombination, std::vector<std::vector<Card>>& possiblePlays)
{
    for (const Card& card : hand)
    {
        if (currentCombination.power > 18) //If it's a dragon
            return; //No single card combination can beat a dragon

        if (card.colour == CC_Dog)
        {
            continue;
        }
        
        if (card.colour == CC_Phoenix)
        {
            //No need to check for duplicates, only one Phoenix card in the game
            std::vector<Card> temp{card};
            if (!TestCombinationValidity(temp))
            {
                __debugbreak();
            }
            possiblePlays.emplace_back(temp);
        }

        //Special edge case for the phoenix
        else if (currentCombination.combinationType == CombinationType::CT_SinglePhoenix)
        {
            if (card.power >= currentCombination.power)
            {
                if (std::find(possiblePlays.begin(), possiblePlays.end(), std::vector<Card>{card}) == possiblePlays.end())
                {
                    std::vector<Card> temp{card};
                    if (!TestCombinationValidity(temp))
                    {
                        __debugbreak();
                    }
                    possiblePlays.emplace_back(temp);
                }                
            }
        }

        else if (card.power > currentCombination.power)
        {
            if (std::find(possiblePlays.begin(), possiblePlays.end(), std::vector<Card>{card}) == possiblePlays.end())
            {
                std::vector<Card> temp{card};
                if (!TestCombinationValidity(temp))
                {
                    __debugbreak();
                }
                possiblePlays.emplace_back(temp);
            }
        }
    }
}

void GenerateDoubleCardPlays(const std::vector<Card>& hand, const Combination& currentCombination, std::vector<std::vector<Card>>& possiblePlays)
{
    std::unordered_map<uint8_t, std::vector<Card>> cardGroups;
    bool hasPhoenix = false;
    Card phoenixCard{CC_Phoenix, 0};

    for (const Card& card : hand)
    {
        cardGroups[card.power].push_back(card);
        if (card.colour == CC_Phoenix)
        {
            hasPhoenix = true;
            phoenixCard = card;
        }
    }

    // Natural pairs
    for (const auto& group : cardGroups)
    {
        uint8_t p = group.first;
        if (group.second.size() >= 2 && p > currentCombination.power)
        {
            std::vector<Card> temp = { group.second[0], group.second[1] };
            if (!TestCombinationValidity(temp))
            {
                __debugbreak();
            }
            possiblePlays.push_back({ group.second[0], group.second[1] });
        }
    }

    // Phoenix pairs: If we have Phoenix and at least one non-phoenix card
    // Conditions:
    // - hasPhoenix
    // - power > currentCombination.power
    // - power != phoenixCard.power
    // - power != 1 (mahjong)
    // - power != 20 (dragon)
    // - at least one real card (non-phoenix) of that power
    if (hasPhoenix)
    {
        for (const auto& group : cardGroups)
        {
            uint8_t p = group.first;
            if (p > currentCombination.power && p != phoenixCard.power && p != 1 && p != 20)
            {
                // Find a non-Phoenix card
                Card nonPhoenixCard{CardColour::CC_Red, 0};
                bool foundNonPhoenix = false;
                for (auto &c : group.second) {
                    if (c.colour != CC_Phoenix) {
                        nonPhoenixCard = c;
                        foundNonPhoenix = true;
                        break;
                    }
                }

                if (foundNonPhoenix) {
                    // Form Phoenix pair: Phoenix at front
                    std::vector<Card> pairPlay;
                    pairPlay.push_back(phoenixCard);
                    pairPlay.push_back(nonPhoenixCard);
                    possiblePlays.push_back(pairPlay);
                    
                    if (!TestCombinationValidity(pairPlay))
                    {
                        __debugbreak();
                    }
                }
            }
        }
    }
}

void GenerateTripleCardPlays(const std::vector<Card>& hand, const Combination& currentCombination, std::vector<std::vector<Card>>& possiblePlays)
{
    std::unordered_map<uint8_t, std::vector<Card>> cardGroups;
    bool hasPhoenix = false;
    Card phoenixCard{CC_Phoenix, 0};

    for (const Card& card : hand)
    {
        cardGroups[card.power].push_back(card);
        if (card.colour == CC_Phoenix)
        {
            hasPhoenix = true;
            phoenixCard = card;
        }
    }

    // Natural triples
    for (const auto& group : cardGroups)
    {
        uint8_t p = group.first;
        if (group.second.size() >= 3 && p > currentCombination.power)
        {
            std::vector<Card> temp = { group.second[0], group.second[1], group.second[2] };
            if (!TestCombinationValidity(temp))
            {
                __debugbreak();
            }
            possiblePlays.push_back({ group.second[0], group.second[1], group.second[2] });
        }
    }

    // Phoenix triples: 2 identical non-phoenix cards + Phoenix
    if (hasPhoenix)
    {
        for (const auto& group : cardGroups)
        {
            uint8_t p = group.first;
            const auto &cards = group.second;
            if (p > currentCombination.power && p != phoenixCard.power && p != 1)
            {
                // Need at least 2 non-phoenix cards of that power
                std::vector<Card> nonPhoenixCards;
                for (auto &c : cards) {
                    if (c.colour != CC_Phoenix) {
                        nonPhoenixCards.push_back(c);
                    }
                }

                if (nonPhoenixCards.size() >= 2)
                {
                    std::vector<Card> triplePlay;
                    // Phoenix at front
                    triplePlay.push_back(phoenixCard);
                    triplePlay.push_back(nonPhoenixCards[0]);
                    triplePlay.push_back(nonPhoenixCards[1]);
                    possiblePlays.push_back(triplePlay);
                    
                    if (!TestCombinationValidity(triplePlay))
                    {
                        __debugbreak();
                    }
                }
            }
        }
    }
}


void GenerateBombPlays(const std::vector<Card>& hand, const Combination& currentCombination, std::vector<std::vector<Card>>& possiblePlays)
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
            if (currentCombination.combinationType == CombinationType::CT_StraightBomb) return; // No bomb can beat a straight bomb except for a better straight bomb
            if (currentCombination.combinationType == CombinationType::CT_FourOfAKindBomb && group.first <= currentCombination.power) return; // Only bombs of higher power can beat a bomb
            
            possiblePlays.push_back({ group.second[0], group.second[1], group.second[2], group.second[3] });

            std::vector<Card> temp = { group.second[0], group.second[1], group.second[2], group.second[3] };
            if (!TestCombinationValidity(temp))
            {
                __debugbreak();
            }
        }
    }
}

void GenerateFullHousePlays(const std::vector<Card>& hand, const Combination& currentCombination, std::vector<std::vector<Card>>& possiblePlays)
{
    std::unordered_map<uint8_t, std::vector<Card>> cardGroups;
    bool hasPhoenix = false;
    Card phoenixCard{CC_Phoenix, 0};

    for (const Card& card : hand)
    {
        if (card.colour == CC_Dog) continue;
        
        cardGroups[card.power].push_back(card);
        if (card.colour == CC_Phoenix)
        {
            hasPhoenix = true;
        }
    }

    // A small struct to hold triple or pair candidates
    struct Candidate {
        uint8_t power;
        std::vector<Card> cards;
        bool usesPhoenix;
    };

    std::vector<Candidate> tripleCandidates;
    std::vector<Candidate> pairCandidates;

    // Build triple candidates
    for (const auto& group : cardGroups)
    {
        uint8_t p = group.first;
        const auto &cards = group.second;
        size_t count = cards.size();

        // Natural triple
        if (count >= 3)
        {
            // Take first 3 cards
            Candidate c{p, {cards[0], cards[1], cards[2]}, false};
            tripleCandidates.push_back(c);
        }

        // Phoenix triple: 2 real cards + phoenix
        // Conditions:
        // - hasPhoenix
        // - at least 2 non-phoenix cards of same power
        if (hasPhoenix && p != phoenixCard.power && p != 1 && count >= 2)
        {
            // Take the first two cards as the "real" part
            // Add Phoenix
            Candidate c{p, {cards[0], cards[1], phoenixCard}, true};
            tripleCandidates.push_back(c);
        }
    }

    // Build pair candidates
    for (const auto& group : cardGroups)
    {
        uint8_t p = group.first;
        const auto &cards = group.second;
        size_t count = cards.size();

        // Natural pair
        if (count >= 2)
        {
            Candidate c{p, {cards[0], cards[1]}, false};
            pairCandidates.push_back(c);
        }

        // Phoenix pair: 1 real card + phoenix
        // Conditions:
        // - hasPhoenix
        // - not the phoenix power itself
        // - not mahjong (p != 1)
        // - at least 1 non-phoenix card
        if (hasPhoenix && p != phoenixCard.power && p != 1 && count >= 1)
        {
            // Find a non-phoenix card to pair with phoenix
            // Usually cards[0] should be non-phoenix, but check just in case:
            Card singleCard = cards[0];
            if (singleCard.colour == CC_Phoenix && cards.size() > 1) {
                singleCard = cards[1]; 
            }

            // Ensure singleCard is not Phoenix (must have at least one real card)
            if (singleCard.colour != CC_Phoenix) {
                Candidate c{p, {singleCard, phoenixCard}, true};
                pairCandidates.push_back(c);
            }
        }
    }

    // Combine triples and pairs to form full houses
    for (const auto& triple : tripleCandidates)
    {
        // Full house power is determined by the triple's power
        if (triple.power <= currentCombination.power) 
            continue;

        for (const auto& pair : pairCandidates)
        {
            // The triple and pair must have different powers
            if (pair.power == triple.power) 
                continue;

            // Check phoenix usage: only once per combination
            if (triple.usesPhoenix && pair.usesPhoenix) 
                continue;

            // Form the full house: triple + pair
            std::vector<Card> fullHouse;
            fullHouse.insert(fullHouse.end(), triple.cards.begin(), triple.cards.end());
            fullHouse.insert(fullHouse.end(), pair.cards.begin(), pair.cards.end());

            // If phoenix is used in either triple or pair, move phoenix to front
            if (triple.usesPhoenix || pair.usesPhoenix)
            {
                auto it = std::find_if(fullHouse.begin(), fullHouse.end(), [&](const Card &c){
                    return c.colour == CC_Phoenix;
                });
                if (it != fullHouse.end()) {
                    Card ph = *it;
                    fullHouse.erase(it);
                    fullHouse.insert(fullHouse.begin(), ph);
                }
            }

            possiblePlays.push_back(fullHouse);
            
            if (!TestCombinationValidity(fullHouse))
            {
                __debugbreak();
            }
        }
    }
}


void GenerateFilledStraights(const std::vector<Card>& hand, const Combination& currentCombination, std::vector<std::vector<Card>>& possiblePlays)
{
    // 1) Separate the Phoenix card and collect normal cards
    Card phoenixCard{CardColour::CC_Phoenix, 0};           
    bool hasPhoenix = false;

    std::set<uint8_t> uniquePowers;
    std::unordered_map<uint8_t, std::vector<Card>> cardGroups;

    for (const Card& card : hand)
    {
        if (card.colour == CC_Dog) continue;
        
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

            // Once we have a new card in straight,
            // check if it's a valid straight that can beat the current combination
            if (straight.size() >= 5 &&
                (currentCombination.combinationType == CombinationType::CT_Invalid
                    || (straight.size() == currentCombination.numberOfCards && straight.front().power > currentCombination.power)))
            {
                const int powerToCompare = usedPhoenix ? straight[1].power : straight.front().power;
                if (powerToCompare > currentCombination.power)
                {
                    possiblePlays.push_back(straight);
                }
                
                if (!TestCombinationValidity(straight))
                {
                    __debugbreak();
                }
            }
        }
    }
}


void GeneratePureStraights(const std::vector<Card>& hand, std::vector<std::vector<Card>>& possiblePlays)
{
    // Collect unique card powers
    std::set<uint8_t> uniquePowers;
    std::unordered_map<uint8_t, std::vector<Card>> cardGroups;
    for (const Card& card : hand)
    {
        if (card.colour == CC_Dog || card.colour == CC_Phoenix) continue;
        
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

                if (straight.size() >= 5)
                {
                    possiblePlays.push_back(straight);

                    if (!TestCombinationValidity(straight))
                    {
                        __debugbreak();
                    }
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
    Card phoenixCard{CardColour::CC_Phoenix, 0};
    for (const Card& card : hand)
    {        
        if (card.colour == CC_Phoenix)
        {
            hasPhoenix = true;
            break;
        }
    }

    std::vector<std::vector<Card>> pureStraights{};
    GeneratePureStraights(hand, pureStraights);
        
        
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
            
            if (currentCombination.combinationType == CombinationType::CT_Invalid || //If it's the first play and the combination is valid you can play it
                (moveWithPhoenix.front().power > currentCombination.power && //Otherwise you need to beat the other one
                moveWithPhoenix.size() == currentCombination.numberOfCards)) //and you need to match the number of cards
            {
                possiblePlays.push_back(moveWithPhoenix);

                if (!TestCombinationValidity(moveWithPhoenix))
                {
                    __debugbreak();
                }
            }
        }
    }
    else
    {
        for (const std::vector<Card>& straight : pureStraights)
        {
            if (currentCombination.combinationType == CombinationType::CT_Invalid || //If it's the first play and the combination is valid you can play it
                (straight[0].power > currentCombination.power && //Otherwise you need to beat the other one
                    straight.size() == currentCombination.numberOfCards)) //and you need to match the number of cards
            {
                possiblePlays.push_back(straight);
            }
        }
    }
        
}


static std::string CardsToString(const std::vector<Card>& cards)
{
    std::vector<int> ids;
    for (auto &c : cards) ids.push_back(c.id);
    std::sort(ids.begin(), ids.end());
    std::string result;
    for (auto id : ids) {
        result += std::to_string(id) + "-";
    }
    return result;
}

void GenerateStepPlays(const std::vector<Card>& hand, const Combination& currentCombination, std::vector<std::vector<Card>>& possiblePlays)
{
    bool hasPhoenix = false;
    Card phoenixCard{CC_Phoenix, 0};
    std::unordered_map<uint8_t, std::vector<Card>> cardGroups;

    for (const Card& card : hand)
    {
        cardGroups[card.power].push_back(card);
        if (card.colour == CC_Phoenix)
        {
            hasPhoenix = true;
            phoenixCard = card;
        }
    }

    struct PairOption
    {
        uint8_t power;
        bool naturalPairPossible;
        bool phoenixPairPossible;
    };

    std::vector<PairOption> allPairs; 

    // Identify which powers can form which kind of pairs
    for (const auto& group : cardGroups)
    {
        uint8_t p = group.first;
        size_t count = group.second.size();

        bool natural = (count >= 2);
        bool phoenix = false;
        // Phoenix pair only if:
        // - we have phoenix
        // - power != phoenix power
        // - power != 1 (mahjong can't form phoenix pairs)
        // - we have at least one non-phoenix card of that power
        if (hasPhoenix && p != phoenixCard.power && p != 1) { 
            if ((count >= 1) && !(count == 1 && group.second[0].colour == CC_Phoenix)) {
                phoenix = true;
            }
        }
        if (natural || phoenix) {
            allPairs.push_back({p, natural, phoenix});
        }
    }

    std::sort(allPairs.begin(), allPairs.end(), [](const PairOption &a, const PairOption &b) {
        return a.power < b.power;
    });

    // Find sequences of at least 2 consecutive powers
    std::vector<std::vector<PairOption>> sequences;
    for (size_t i = 0; i < allPairs.size(); ++i)
    {
        std::vector<PairOption> seq;
        seq.push_back(allPairs[i]);
        uint8_t prev = allPairs[i].power;
        for (size_t j = i+1; j < allPairs.size(); ++j)
        {
            if (allPairs[j].power == prev + 1)
            {
                seq.push_back(allPairs[j]);
                prev = allPairs[j].power;
            } else
            {
                break;
            }
        }
        if (seq.size() >= 2)
        {
            sequences.push_back(seq);
        }
    }

    std::set<std::string> uniqueResults;

    auto recordCombination = [&](const std::vector<Card>& combination)
    {
        // combination may need phoenix at front if it exists
        std::vector<Card> finalCombo = combination;
        auto it = std::find_if(finalCombo.begin(), finalCombo.end(), [&](const Card &c)
        {
            return c.colour == CC_Phoenix;
        });
        bool hasPhoenixInThisCombo = (it != finalCombo.end());
        if (hasPhoenixInThisCombo) {
            Card ph = *it;
            finalCombo.erase(it);
            finalCombo.insert(finalCombo.begin(), ph);
        }

        // Check power
        // The step's power is defined by the lowest pair in the step,
        // i.e., the first pair's power. That's finalCombo's second card's power if Phoenix at front:
        // Actually, we know these are pairs. If Phoenix is at front, the second card is the pair's other half.
        // The lowest pair's power will be the minimum pair's power in sequence, but we built them in order.
        // The first pair is always the first 2 cards after rearrangement:
        uint8_t stepPower = finalCombo[ (hasPhoenixInThisCombo ? 1 : 0) ].power;
        if (stepPower <= currentCombination.power || finalCombo.size() != currentCombination.numberOfCards) return;

        std::string sig = CardsToString(finalCombo);
        if (uniqueResults.find(sig) == uniqueResults.end())
        {
            uniqueResults.insert(sig);
            possiblePlays.push_back(finalCombo);
            
            if (!TestCombinationValidity(finalCombo))
            {
                __debugbreak();
            }
        }
    };

    // Backtracking to try all combos of natural/phoenix pairs
    std::function<void(size_t, bool, const std::vector<PairOption>&, std::vector<Card>)> backtrack;
    backtrack = [&](size_t idx, bool phoenixUsed, const std::vector<PairOption>& seq, std::vector<Card> currentStep) {
        if (idx == seq.size()) {
            // End of sequence
            return; // We record combos as we go, not just at the end
        }

        const PairOption &po = seq[idx];
        const auto &cards = cardGroups.at(po.power);

        // Try natural variant if possible
        if (po.naturalPairPossible) {
            std::vector<Card> newStep = currentStep;
            newStep.push_back(cards[0]);
            newStep.push_back(cards[1]);

            // Record if at least 2 pairs formed
            if (newStep.size() >= 4) {
                recordCombination(newStep);
            }

            // Continue
            backtrack(idx+1, phoenixUsed, seq, newStep);
        }

        // Try phoenix variant if possible and not used
        if (po.phoenixPairPossible && !phoenixUsed) {
            // Find a non-phoenix card
            Card singleCard = cards[0];
            if (singleCard.colour == CC_Phoenix && cards.size() > 1) {
                singleCard = cards[1];
            }
            std::vector<Card> newStep = currentStep;
            newStep.push_back(singleCard);
            newStep.push_back(phoenixCard);

            if (newStep.size() >= 4) {
                recordCombination(newStep);
            }

            backtrack(idx+1, true, seq, newStep);
        }
    };

    for (auto &seq : sequences) {
        backtrack(0, false, seq, {});
    }
}


void GeneratePossiblePlays(const std::vector<Card>& hand, const Combination& currentCombination, std::vector<std::vector<Card>>& possiblePlays)
{
    switch (currentCombination.combinationType)
    {
    case CombinationType::CT_SinglePhoenix:
    case CombinationType::CT_Single:
        GenerateSingleCardPlays(hand, currentCombination, possiblePlays);
        //Check if you can find a card with colour CC_Dog inside the possible plays
        for (const auto& play : possiblePlays)
        {
            for (const auto& card : play)
            {
                if (card.colour == CC_Dog)
                {
                    __debugbreak();
                }
            }
        }
        break;
    case CombinationType::CT_Doubles:
        GenerateDoubleCardPlays(hand, currentCombination, possiblePlays);
        for (const auto& play : possiblePlays)
        {
            for (const auto& card : play)
            {
                if (card.colour == CC_Dog)
                {
                    __debugbreak();
                }
            }
        }
        break;
    case CombinationType::CT_Triples:
        GenerateTripleCardPlays(hand, currentCombination, possiblePlays);
        for (const auto& play : possiblePlays)
        {
            for (const auto& card : play)
            {
                if (card.colour == CC_Dog)
                {
                    __debugbreak();
                }
            }
        }
        break;
    case CombinationType::CT_FullHouse:
        GenerateFullHousePlays(hand, currentCombination, possiblePlays);
        for (const auto& play : possiblePlays)
        {
            for (const auto& card : play)
            {
                if (card.colour == CC_Dog)
                {
                    __debugbreak();
                }
            }
        }
        break;
    case CombinationType::CT_Straight:
        GenerateStraightPlays(hand, currentCombination, possiblePlays);
        for (const auto& play : possiblePlays)
        {
            for (const auto& card : play)
            {
                if (card.colour == CC_Dog)
                {
                    __debugbreak();
                }
            }
        }
        break;
    case CombinationType::CT_Steps:
        GenerateStepPlays(hand, currentCombination, possiblePlays);
        for (const auto& play : possiblePlays)
        {
            for (const auto& card : play)
            {
                if (card.colour == CC_Dog)
                {
                    __debugbreak();
                }
            }
        }
        break;
    case CombinationType::CT_Dogs: //When it's dogs it's the same as invalid (aka empty table)
    case CombinationType::CT_Invalid:
        GenerateSingleCardPlays(hand, currentCombination, possiblePlays);
        for (const auto& play : possiblePlays)
        {
            for (const auto& card : play)
            {
                if (card.colour == CC_Dog)
                {
                    __debugbreak();
                }
            }
        }
        GenerateDoubleCardPlays(hand, currentCombination, possiblePlays);
        for (const auto& play : possiblePlays)
        {
            for (const auto& card : play)
            {
                if (card.colour == CC_Dog)
                {
                    __debugbreak();
                }
            }
        }
        GenerateTripleCardPlays(hand, currentCombination, possiblePlays);
        for (const auto& play : possiblePlays)
        {
            for (const auto& card : play)
            {
                if (card.colour == CC_Dog)
                {
                    __debugbreak();
                }
            }
        }
        GenerateFullHousePlays(hand, currentCombination, possiblePlays);
        for (const auto& play : possiblePlays)
        {
            for (const auto& card : play)
            {
                if (card.colour == CC_Dog)
                {
                    __debugbreak();
                }
            }
        }
        GenerateStraightPlays(hand, currentCombination, possiblePlays);
        for (const auto& play : possiblePlays)
        {
            for (const auto& card : play)
            {
                if (card.colour == CC_Dog)
                {
                    __debugbreak();
                }
            }
        }
        GenerateStepPlays(hand, currentCombination, possiblePlays);

        for (const auto& play : possiblePlays)
        {
            for (const auto& card : play)
            {
                if (card.colour == CC_Dog)
                {
                    __debugbreak();
                }
            }
        }

        if (currentCombination.combinationType != CombinationType::CT_Invalid) break;
        
        // Dogs can only be played when no other cards are on the table
        for (const auto& card : hand)
        {
            if (card.colour == CC_Dog)
            {
                possiblePlays.push_back(std::vector<Card>{card});
            }
        }
        break;
    case CombinationType::CT_FourOfAKindBomb:
        GenerateBombPlays(hand, currentCombination, possiblePlays);
        break;
    case CombinationType::CT_StraightBomb:
        //todo: later
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
    GeneratePossiblePlays(hand, currentState.currentCombination, possiblePlays);

    // Safe removal using the remove-erase idiom
    // Remove any plays that contain a dog card if the current combination is not invalid
    // if (currentCombination.combinationType != CombinationType::CT_Invalid)
    // {
    //     std::erase_if(
    //         possiblePlays,
    //         [](const auto& play)
    //         {
    //             // If this play contains a CC_Dog card, we remove it
    //             for (const auto& card : play)
    //             {
    //                 if (card.colour == CC_Dog)
    //                 {
    //                     return true; // Mark for removal
    //                 }
    //             }
    //             return false; // Keep this play
    //         }
    //     );
    // }
    //
    // // Delete any plays that are over 1 card long and contain a dog
    // // These somehow slipped through the cracks
    // std::erase_if(possiblePlays, [](const std::vector<Card>& play)
    // {
    //     if (play.size() > 1)
    //     {
    //         for (const auto& card : play)
    //         {
    //             if (card.colour == CC_Dog)
    //             {
    //                 return true; // Mark this for removal
    //             }
    //         }
    //     }
    //
    //     return false;
    // });

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
            if (cardsOnTable.size() > 1 && cardsOnTable[cardsOnTable.size() - 1].colour == CC_Dragon)
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

                if (is1_2Finish)
                {
                    return true;
                }
            }
        }
    }
    
    const bool isTerminal = playersOut >= 3;
    return isTerminal;
}

#pragma endregion

}