#ifndef CARD_H
#define CARD_H

#include <iosfwd>
#include <QObject>
#include <QGraphicsPixmapItem>

#include "suit.hpp"

#include "seat.hpp"

class QGraphicsSceneMouseEvent;
class Player;

class Card : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT
    Q_PROPERTY(QPointF position READ pos WRITE setPos)
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)

public:
    static const char className[];

    enum Value { TWO=2, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, JACK, QUEEN, KING, ACE, LEFT_BOWER, RIGHT_BOWER, JOKER };
    enum Location { HAND, TRICK, HIDDEN };

    static const unsigned H_OFFSET = 20;
    static const unsigned HEIGHT = 116;
    static const unsigned WIDTH = 80;
    static const unsigned EDGE_OFFSET = 100;
    static const unsigned RAISE = 20;
    static const unsigned WON_OFFSET = 160;

    Card();
    Card(Suit suit, Value val);
    Card& operator=(const Card& copy);
    bool operator==(const Card& other) const;
    virtual ~Card();

    void reposition(QSize screen);


    void setLocation(Location l) const;
    void setHandOffset(unsigned i, unsigned sz) const;
    void setFromSeat(Seat s) const;
    void setFaceUp(bool setFaceUp=true) const;
    void raise(bool v=true) const;
    bool raised() const;

    bool isTrump(Suit trumps) const;
    Suit suit(Suit trump = Suit::NONE) const;
    Value value(Suit trump = Suit::NONE) const;

    class CompareInHand {
    public:
        CompareInHand(Suit trumps) : m_trumps(trumps) {}
        bool operator()(const Card* lhs, const Card* rhs);
    private:
        Suit m_trumps;
    };


    QPointF expectedPosition(QSize screen, Location loc) const;

signals:
    void cardClicked(Card& card);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent * event);
    Suit m_suit;
    Value m_value;

    struct Display {
        Location location;
        double relative_offset;
        Seat from_seat;
        bool raised;
        bool faceup;
    };

    mutable Display m_new_display;
    Display m_old_display;

    static QString findImage(Suit suit, int value);

    friend std::ostream& operator<<(std::ostream& s, const Card& c);
};

std::istream& operator>>(std::istream& s, Card& c);

#endif // CARD_H
