#ifndef SUIT_HPP
#define SUIT_HPP

#include <iosfwd>
#include <cassert>

class Suit {
public:
    static const char className[];

    enum SuitEnum { SPADES=0, DIAMONDS, CLUBS, HEARTS, NONE };
    Suit(SuitEnum suit = NONE) : m_suitenum(suit) {}

    bool operator==(const Suit& other) const {
        return m_suitenum == other.m_suitenum;
    }
    bool operator==(SuitEnum suit) const {
        return m_suitenum == suit;
    }
    bool operator!=(SuitEnum suit) const {
        return !operator==(suit);
    }
    bool operator<(const Suit& other) const {
        if(*this == other) return false;
        if(m_suitenum == SPADES) return true;
        if(m_suitenum == CLUBS) return other.m_suitenum != SPADES;
        if(m_suitenum == DIAMONDS) return (other.m_suitenum == HEARTS) | (other.m_suitenum == NONE);
        if(m_suitenum == HEARTS) return other.m_suitenum == NONE;
        if(m_suitenum == NONE) return false;
        assert(0);
    }
    operator int() const {
        return int(m_suitenum);
    }

    Suit Complementary() const {
        switch(m_suitenum) {
            case SPADES:    return Suit(CLUBS);     break;
            case CLUBS:     return Suit(SPADES);    break;
            case DIAMONDS:  return Suit(HEARTS);    break;
            case HEARTS:    return Suit(DIAMONDS);  break;
            default:        return Suit(NONE);      break;
        }
    }

    static struct HandComparator {
        bool operator()(const Suit& lhs, const Suit& rhs) const;
    } handcomparator;

    friend std::ostream& operator<<(std::ostream& s, const Suit& suit);
    friend std::istream& operator>>(std::istream& s, Suit& suit);
private:
    SuitEnum m_suitenum;
};
#endif // SUIT_HPP
