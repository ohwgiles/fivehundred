#ifndef SIMPLECARD_HPP
#define SIMPLECARD_HPP

#include "suit.hpp"

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
