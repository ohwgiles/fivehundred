#include "simplecard.hpp"
/*!
  \file simplecard.cpp
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

#include "log.hpp"

const char SimpleCard::className[] = "Card";

SimpleCard::SimpleCard(Suit suit, Value val):
    m_suit(suit),
    m_value(val)
{
    trace;
    if(suit == Suit::NONE && val == INVALID) {} else
    //if(suit == Suit::NONE && val == JOKER) {} else
    if(val >= TWO && val <= JOKER) {} else
        fatal(error<<"Invalid parameters to Card ctor: " << val << "," << suit);
}

void SimpleCard::setJokerSuit(Suit suit) {
    trace;
    if(m_value != JOKER) {
        error << "Cannot set suit of non-joker card";
        return;
    }

    debug << "Setting joker to suit: " << suit;
    m_suit = suit;
}

bool SimpleCard::operator==(const SimpleCard& other) const {
    trace;
    Value myValue = m_value, otherValue = other.m_value;
    Suit mySuit = m_suit, otherSuit = other.m_suit;
    if(myValue == JOKER) return otherValue == JOKER;
    if(myValue == LEFT_BOWER) { myValue = JACK; mySuit = mySuit.complementary(); }
    if(otherValue == LEFT_BOWER) { otherValue = JACK; otherSuit = otherSuit.complementary(); }
    if(myValue == RIGHT_BOWER) myValue = JACK;
    if(otherValue == RIGHT_BOWER) otherValue = JACK;
    return (m_suit == other.m_suit && m_value == other.m_value);
}

Suit SimpleCard::suit(Suit trump) const {
    trace << "Getting suit of " << *this << " with trump suit " << trump;
    if(m_value == JOKER && trump != Suit::NONE) return trump;
    if(trump != Suit::NONE && m_value == JACK && m_suit == trump.complementary()) return trump;
    return m_suit;
}

SimpleCard::Value SimpleCard::value(Suit trump) const {
    trace << "Getting value of " << *this << " with trump suit " << trump;
    if (m_value == JACK && suit(trump) == trump)
        return m_suit == trump.complementary() ? LEFT_BOWER : RIGHT_BOWER;
    return m_value;
}

std::ostream& operator<<(std::ostream& s, const SimpleCard& c) {
    trace;
    if(c.m_value == SimpleCard::JOKER) {
        s << "Joker (" << c.m_suit << ")";
        return s;
    } else if(c.m_value < SimpleCard::JACK)
        s << c.m_value << " of ";
    else if(c.m_value == SimpleCard::JACK)
        s << "Jack of ";
    else if(c.m_value == SimpleCard::QUEEN)
        s << "Queen of ";
    else if(c.m_value == SimpleCard::KING)
        s << "King of ";
    else if(c.m_value == SimpleCard::ACE)
        s << "Ace of ";
    s << c.m_suit;
    return s;
}

std::istream& operator>>(std::istream& is, SimpleCard& c) {
    trace;
    std::string tmp;

    is >> tmp;
    if(*tmp.rbegin() == ',')
        tmp.erase(tmp.end()-1);

    SimpleCard::Value v;
    if(tmp == "Joker") {
        c = SimpleCard(Suit::NONE, SimpleCard::JOKER);
        return is;
    } else if(tmp == "Jack") {
        v = SimpleCard::JACK;
    } else if(tmp == "Queen") {
        v = SimpleCard::QUEEN;
    } else if(tmp == "King") {
        v = SimpleCard::KING;
    } else if(tmp == "Ace") {
        v = SimpleCard::ACE;
    } else {
        std::stringstream ss(tmp);
        unsigned tmp_value;
        ss >> tmp_value;
        if(!(tmp_value >= SimpleCard::TWO && tmp_value <= SimpleCard::TEN)) {
            error << "Failed to deserialise card value";
            is.setstate(is.failbit);
            return is;
        } else {
            v = SimpleCard::Value(tmp_value);
        }
    }

    is >> tmp;
    if(tmp != "of") {
        error << "Expected: 'of'";
        is.setstate(is.badbit);
    } else {
        Suit e;
        if (!(is >> e).fail()) {
            c = SimpleCard(e, v);
        }
    }
    return is;
}
