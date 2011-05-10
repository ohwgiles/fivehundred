#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <vector>
#include <iosfwd>
#include "suit.hpp"

#include "bid.hpp"
#include "seat.hpp"

#define CARD_RAISE 20

class Card;
class Trick;
class Bidding;
#include <QGraphicsTextItem>

typedef std::vector<Card*> Hand;
class Player : public QObject
{
    Q_OBJECT
public:
    static const char className[];

    Player(Seat pos, const QString& name);
    virtual void configure() {}

    Hand hand;

    virtual void abort() {}
    virtual void handDealt() = 0;
    virtual Card* yourTurnToPlay(const Trick*) = 0;
    virtual Bid yourTurnToBid(const Bidding* bidlist) = 0;
    virtual void bidWon(const Bidding* bidlist, const Player* winner) = 0;
    virtual Hand yourTurnToSelectKitty(const Hand& kitty) = 0;

    virtual void trickWon(const Trick& trick, const Player* winner) = 0;

    bool operator==(const Player& other) { return this == &other; }

    Seat pos() const { return m_pos; }

    //int m_tricksWon;
    const QString name;
    QGraphicsTextItem m_player_ui_label;

    Player* next; // linked-list behaviour

signals:
    void kittyChosen(const Hand&);
    void acknowledgeTrick(Player* self);

protected:
    bool cardValid(const Trick* trick, Suit trumps, Card& card);

    void layoutHand();

    Seat m_pos;
};


std::istream& operator>>(std::istream& s, Player& player);
std::ostream& operator<<(std::ostream& s, const Player& player);

#endif // PLAYER_HPP
