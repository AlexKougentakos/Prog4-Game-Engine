#pragma once
#include <vector>

struct Card;

namespace ody
{
    class EventData
    {
    public:
        virtual ~EventData() = default;
    };

    class CardEventData : public EventData
    {
    public:
        explicit CardEventData(const std::vector<Card>& cards) : playedCards(cards) {}
        std::vector<Card> playedCards;
    };

    class DragonEventData : public EventData
    {
    public:
        explicit DragonEventData(const int playerID) : recipientPlayerID(playerID) {}
        int recipientPlayerID;
    };
}