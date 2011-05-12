#ifndef BID_HPP
#define BID_HPP
/*!
  \file bid.hpp
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
#include "suit.hpp"

class Bid {
public:
    static const char className[];

    enum Type { PASS, NORMAL, CLOSED_MISERE, OPEN_MISERE };

    Bid(Type bid = PASS);
    Bid(Suit suit, int numTricks);

    bool operator <(const Bid& other) const;
    bool operator ==(const Bid& other) const;

    friend std::ostream& operator<<(std::ostream& s, const Bid& bid);
    friend std::istream& operator>>(std::istream& s, Bid& bid);

    Type type() const { return m_type; }
    const Suit& suit() const { return m_suit; }
    int tricks() const { return m_type == NORMAL ? m_tricks : 0; }
    int worth() const;

private:

    Type m_type;
    Suit m_suit;
    int m_tricks;

};

#endif // BID_HPP
