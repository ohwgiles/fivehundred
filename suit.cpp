#include "suit.hpp"
/*!
  \file suit.cpp
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
#include <iostream>
#include "log.hpp"

const char Suit::className[] = "Suit";

std::ostream& operator<<(std::ostream& s, const Suit& suit) {
    trace;
    switch(suit.m_suitenum) {
    case Suit::SPADES:
        s << "Spades";
        break;
    case Suit::CLUBS:
        s << "Clubs";
        break;
    case Suit::DIAMONDS:
        s << "Diamonds";
        break;
    case Suit::HEARTS:
        s << "Hearts";
        break;
    case Suit::NONE:
        s << "No Trumps";
        break;
    }
    return s;
}


std::istream& operator>>(std::istream& s, Suit& suit) {
    trace;
    std::string str;
    s >> str;
    if(str == "Spades")
        suit = Suit(Suit::SPADES);
    else if(str == "Clubs")
        suit = Suit(Suit::CLUBS);
    else if(str =="Diamonds")
        suit = Suit(Suit::DIAMONDS);
    else if(str == "Hearts")
        suit = Suit(Suit::HEARTS);
    else if(str == "No") {
        std::string check;
        s >> check;
        if(check == "Trumps") suit = Suit(Suit::NONE);
        else s.setstate(s.badbit);
    } else
        s.setstate(std::ios_base::failbit);
    return s;
}
