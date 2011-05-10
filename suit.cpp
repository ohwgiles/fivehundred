#include "suit.hpp"
#include <iostream>
#include <cassert>
#include "log.hpp"

const char Suit::className[] = "Suit";

Suit::HandComparator Suit::handcomparator;

bool Suit::HandComparator::operator ()(const Suit& lhs, const Suit& rhs) const {
    trace;
    //assert(lhs != NONE && rhs != NONE);
    if(lhs == rhs) return false;
    if(lhs == SPADES) return true;
    if(lhs == DIAMONDS) return rhs == CLUBS || rhs == HEARTS || rhs == NONE;
    if(lhs == CLUBS) return rhs == HEARTS || rhs == NONE;
    if(lhs == HEARTS) return rhs == NONE;
    return false;
}

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
