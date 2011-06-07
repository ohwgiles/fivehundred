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

/*!
  \class Bid
  \brief Bid object
*/
class Bid {
public:
    //! Every Lua-accessible object must have a string identifier
    static const char className[];

    enum Type { PASS, NORMAL, CLOSED_MISERE, OPEN_MISERE };

    Bid(Type bid = PASS);
    Bid(Suit suit, int numTricks);

    static int suitToInt(Suit s);
    static Suit intToSuit(int i);

    bool operator<(const Bid& other) const;
    bool operator==(const Bid& other) const;

    friend std::ostream& operator<<(std::ostream& s, const Bid& bid);
    friend std::istream& operator>>(std::istream& s, Bid& bid);

    Type type() const { return m_type; }
    const Suit& suit() const;
    int tricks() const;
    int worth() const;

protected:
    Type m_type;
    Suit m_suit; //!< If m_type is NORMAL, the suit bid
    int m_tricks; //!< If m_type is NORMAL, the number of tricks bid
};

#endif // BID_HPP
