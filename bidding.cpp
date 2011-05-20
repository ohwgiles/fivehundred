#include "bidding.hpp"
/*!
  \file bidding.cpp
    Copyright 2011 Oliver Giles

    This file is part of Five Hundred.

    Five Hundred is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Five Hundred is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Five Hundred.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "player.hpp"
#include <algorithm>
#include "log.hpp"


void Bidding::bid(Player* player, Bid bid) {
    trace;
    if(bid == Bid::PASS)
        m_has_passed[player] = true;
    // If it was the maximum bid, everyone else automatically passes
    else if(bid == Bid(Suit::NONE, 10)) {
        m_has_passed[player->next] = true;
        m_has_passed[player->next->next] = true;
        m_has_passed[player->next->next->next] = true;
    } else {
        std::vector<Pair>::iterator it = std::find_if(m_bids.begin(), m_bids.end(), [=](const Pair& p){ return p.player == player; });
        if(it != m_bids.end() && (it->bid.type() != bid.type() || (bid.type() == Bid::NORMAL && bid.suit() != it->bid.suit()))) {
            // everyone's back in the bidding
            m_has_passed[player->next] = false;
            m_has_passed[player->next->next] = false;
            m_has_passed[player->next->next->next] = false;
        }
    }
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
        return std::count_if(m_has_passed.begin(), m_has_passed.end(), [](const PlayerBool& p){ return p.second; } ) == 4;
    else
        return m_bids.size() == 4;
}

Bid Bidding::maxBid() const {
    trace;
    for(std::vector<Pair>::const_reverse_iterator it = m_bids.rbegin(); it != m_bids.rend(); ++it) {
        if(!(it->bid == Bid::PASS)) return it->bid;
    }
    return Bid(Bid::PASS);
}


const Bidding::Pair& Bidding::winner() const {
    trace;
    for(std::vector<Pair>::const_reverse_iterator it = m_bids.rbegin(); it != m_bids.rend(); ++it) {
        if(!((*it).bid == Bid::PASS)) return *it;
    }
    fatal(error << "There is no winner");
}


Player* Bidding::nextBidder(Player* current) {
    trace;
    Player* next = current;
    do { next = next->next; } while(m_has_passed[next] == true);
    return next;
}

