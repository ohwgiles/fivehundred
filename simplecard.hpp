#ifndef SIMPLECARD_HPP
#define SIMPLECARD_HPP
/*!
  \file simplecard.hpp
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
  \class SimpleCard
  \brief A card without any UI information. Used by scripts etc.
*/
class SimpleCard {
public:
    //! Every Lua-accessible object must have a string identifier
    static const char className[];

    enum Value { INVALID=-1, TWO=2, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, JACK, QUEEN, KING, ACE, LEFT_BOWER, RIGHT_BOWER, JOKER };

    SimpleCard(Suit suit = Suit::NONE, Value val = INVALID);

    bool operator==(const SimpleCard& other) const;
    bool operator==(const SimpleCard* other) const { return this->operator==(*other); }

    //bool isTrump(Suit trumps) const;
    Suit suit(Suit trump = Suit::NONE) const;
    Value value(Suit trump = Suit::NONE) const;

    void setJokerSuit(Suit suit);

protected:
    Suit m_suit;
    Value m_value;

    friend std::ostream& operator<<(std::ostream& s, const SimpleCard& c);

};

std::istream& operator>>(std::istream& s, SimpleCard& c);


#endif // SIMPLECARD_HPP
