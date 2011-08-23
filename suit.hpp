#ifndef SUIT_HPP
#define SUIT_HPP
/*!
  \file suit.hpp
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
#include <iosfwd>

/*!
  \class Suit
  \brief Glorified enumeration for handling suits
*/
class Suit {
public:
    //! Every Lua-accessable object must have a string identifier
    static const char className[];

    // Possible values of Suit
    enum SuitEnum { NONE=0, SPADES, DIAMONDS, CLUBS, HEARTS };

    Suit(SuitEnum suit = NONE) : m_suitenum(suit) {}

    bool operator==(const Suit& other) const {
        return m_suitenum == other.m_suitenum;
    }
    bool operator!=(const Suit& other) const {
        return !operator==(other);
    }
    bool operator==(SuitEnum suit) const {
        return m_suitenum == suit;
    }
    bool operator!=(SuitEnum suit) const {
        return !operator==(suit);
    }

    int toInt() const {
        return int(m_suitenum);
    }

    //! Returns the opposite suit (e.g to find bowers)
    Suit complementary() const {
        switch(m_suitenum) {
            case SPADES:    return Suit(CLUBS);     break;
            case CLUBS:     return Suit(SPADES);    break;
            case DIAMONDS:  return Suit(HEARTS);    break;
            case HEARTS:    return Suit(DIAMONDS);  break;
            default:        return Suit(NONE);      break;
        }
    }

    friend std::ostream& operator<<(std::ostream& s, const Suit& suit);
    friend std::istream& operator>>(std::istream& s, Suit& suit);

private:
    SuitEnum m_suitenum;
};
#endif // SUIT_HPP
