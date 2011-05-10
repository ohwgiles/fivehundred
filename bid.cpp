
#include "bid.hpp"
#include <iostream>
#include <boost/foreach.hpp>
#include "player.hpp"
#include "log.hpp"

const char Bid::className[] = "Bid";

Bid::Bid(Type bid) :
        m_type(bid),
        m_suit(Suit::NONE)
{
    trace;

    if(bid == PASS || bid == CLOSED_MISERE || bid == OPEN_MISERE) {} else {
        error << "Unknown bid type: " << bid;
    }
}

Bid::Bid(Suit suit, int numTricks) :
        m_type(NORMAL),
        m_suit(suit),
        m_tricks(numTricks)
{
    trace;
    assert(suit == Suit::SPADES || suit == Suit::CLUBS || suit == Suit::DIAMONDS || suit == Suit::HEARTS || suit == Suit::NONE);
    assert(numTricks >= 5 && numTricks <= 10);
}

bool Bid::operator <(const Bid& other) const {
    trace;
    if(*this == other) return false;
    if((m_type == other.m_type) == NORMAL) {
        if(m_tricks == other.m_tricks) return m_suit < other.m_suit;
        else return m_tricks < other.m_tricks;
    } else {
        if(m_type == PASS) return true;
        if(m_type == NORMAL) {
            if(other.m_type == PASS) return false;
            if(other.m_type == CLOSED_MISERE) return m_tricks <= 7;
            if(other.m_type == OPEN_MISERE) return m_tricks < 10;
        }
        if(m_type == CLOSED_MISERE) {
            if(other.m_type == PASS) return false;
            if(other.m_type == NORMAL) return other.m_tricks > 7;
            if(other.m_type == OPEN_MISERE) return true;
        }
        if(m_type == OPEN_MISERE) {
            if(other.m_type == PASS) return false;
            if(other.m_type == NORMAL) return other.m_tricks == 10;
            if(other.m_type == CLOSED_MISERE) return false;
        }
    }
    fatal(error << "Could not compare bids: " << *this << " and " << other);
}

bool Bid::operator ==(const Bid& other) const {
    trace;
    if(m_type == other.m_type) {
        if(m_type == NORMAL) {
            return m_suit == other.m_suit && m_tricks == other.m_tricks;
        } else return true;
    } else return false;
}

int Bid::worth() const {
    if(m_type == PASS) return 0;
    if(m_type == CLOSED_MISERE) return 250;
    if(m_type == OPEN_MISERE) return 500;
    debug << *this << " is worth " << 100*(m_tricks-6) + 20*(int(m_suit) + 2);
    return 100*(m_tricks-6) + 20*(int(m_suit) + 2);
}


std::ostream& operator<<(std::ostream& s, const Bid& bid) {
    trace;
    switch(bid.m_type) {
    case Bid::PASS:
        s << "Pass";
        break;
    case Bid::NORMAL:
        s << bid.m_tricks << " " << bid.m_suit;
        break;
    case Bid::CLOSED_MISERE:
        s << "Closed Misere";
        break;
    case Bid::OPEN_MISERE:
        s << "Open Misere";
        break;
    }
    return s;
}

std::istream& operator>>(std::istream& is, Bid& bid) {
    trace;
    if(is.peek() >= '0' || is.peek() <= '9') {
        int numtricks;
        is >> numtricks;
        if(!(numtricks >= 6 && numtricks <= 10)) {
            is.setstate(is.badbit);
            return is;
        }
        bid.m_tricks = numtricks;
        is >> bid.m_suit;
    } else {
        std::string str1, str2;
        is >> str1;
        if(str1 == "Pass")
            bid.m_type = Bid::PASS;
        else if(str1 == "Closed" || str1 == "Open") {
            is >> str2;
            if(str2 != "Misere")
                is.setstate(is.badbit);
            else {
                if(str1 == "Closed") bid.m_type = Bid::CLOSED_MISERE;
                if(str1 == "Open") bid.m_type = Bid::OPEN_MISERE;
            }
        } else
            is.setstate(is.badbit);
    }
    return is;
}

