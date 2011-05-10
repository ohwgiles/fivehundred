#include <boost/foreach.hpp>
#include "bidding.hpp"
#include "player.hpp"
#include "log.hpp"

#include <algorithm>

void Bidding::bid(Player* player, Bid bid) {
    trace;
    if(bid == Bid::PASS)
        m_has_passed[player] = true;
    m_bids.push_back(Pair(player, bid));
}


bool Bidding::hasWinner() const {
    trace;
    debug << "bid size:" << m_bids.size();
    if(m_bids.empty())
        return false;
    debug << "number of non-passed bids: " << std::count_if(m_bids.begin(), m_bids.end(), [](const Pair& p){ return !(p.bid == Bid::PASS); });
    if(std::count_if(m_bids.begin(), m_bids.end(), [](const Pair& p){ return !(p.bid == Bid::PASS); }) == 0)
        return false;
    return true;
}

bool Bidding::complete() const {
    trace;
    typedef std::pair<const Player*, bool> PlayerBool;
    debug << "haswinner: " << hasWinner();
    if(hasWinner())
        return std::count_if(m_has_passed.begin(), m_has_passed.end(), [](const PlayerBool& p){ return p.second; } ) == 3;
    else
        return m_bids.size() == 4;
}

Bid Bidding::maxBid() const {
    trace;
    BOOST_REVERSE_FOREACH(const Pair& p, m_bids) {
        if(!(p.bid == Bid::PASS)) return p.bid;
    }
    return Bid(Bid::PASS);
}


const Bidding::Pair& Bidding::winner() const {
    trace;
    BOOST_REVERSE_FOREACH(const Pair& p, m_bids) {
        if(!(p.bid == Bid::PASS)) return p;
    }
    fatal(error << "There is no winner");
}


Player* Bidding::nextBidder(Player* current) {
    trace;
    Player* next = current;
    do { next = next->next; } while(m_has_passed[next] == true);
    return next;
}

