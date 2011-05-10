#ifndef BID_HPP
#define BID_HPP

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
