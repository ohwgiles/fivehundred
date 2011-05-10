#ifndef TRICK_H
#define TRICK_H

#include <vector>
#include "suit.hpp"
#include "seat.hpp"
class Player;
class Card;

class Trick  {
public:
    struct Item {
        Player* player;
        Card* card;
    };

    Trick();
    void playCard(Player* player, Card* card);
    Player* winner(Suit trumps) const;
    bool allPlayed() const { return m_num_cards == 4; }
    int size() const { return m_num_cards; }
    Player* player(int index) const { assert(index < m_num_cards);  return m_plays[index].player; }
    Card* card(int index) const { assert(index < m_num_cards); return m_plays[index].card; }
    std::vector<Card*> cards();

    class CompareInTrick {
    public:
        CompareInTrick(Suit trumps, Suit lead) : m_trumps(trumps), m_lead(lead) {}
        bool operator()(const Card* lhs, const Card* rhs) const;
        bool operator()(const Item& lhs, const Item& rhs) const;
    private:
        Suit m_trumps;
        Suit m_lead;
    };

protected:
    int m_num_cards;
    std::vector<Item> m_plays;
};

#endif // TRICK_H
