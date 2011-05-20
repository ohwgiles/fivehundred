#ifndef BIDDING_HPP
#define BIDDING_HPP
/*!
  \file bidding.hpp
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
#include "bid.hpp"
#include <vector>
#include <map>

//Forward Declarations
class Player;

/*!
  \class Bidding
  \brief Convenience wrapper around a vector of Bid objects
*/
class Bidding  {
public:
    /*!
      \class Pair
      \brief More descriptive than std::pair
    */
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
