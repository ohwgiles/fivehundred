#ifndef BIDDING_HPP
#define BIDDING_HPP

#include <vector>
#include <map>
#include "bid.hpp"

class Player;

class Bidding  {
public:
    struct Pair {
        Pair(Player* player, Bid bid) : player(player), bid(bid) {}
        Pair() : player(0), bid(Bid::PASS) {}
        Player* player;
        Bid bid;
    };
    void bid(Player* player, Bid bid);
    bool hasWinner() const;
    bool complete() const;
    Bid maxBid() const;
    const Pair& winner() const;
    Player* nextBidder(Player* current);
    const Pair& at(int index) const { return m_bids[index]; }
    unsigned count() const { return m_bids.size(); }

private:
    std::map<const Player*, bool> m_has_passed;
    std::vector<Pair> m_bids;
};

#endif // BIDDING_HPP
