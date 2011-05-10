#ifndef HUMAN_H
#define HUMAN_H


#include <QObject>
#include "player.hpp"

#include <QWaitCondition>
#include <QMutex>

class Bid;

class Human : public Player {
    Q_OBJECT
public:

    Human(Seat pos, const QString& name);
    virtual ~Human();

virtual void handDealt();

    virtual Card* yourTurnToPlay(const Trick*);
    virtual Bid yourTurnToBid(const Bidding* bidlist);
    virtual void bidWon(const Bidding* bidlist, const Player* winner);
    virtual Hand yourTurnToSelectKitty(const Hand& kitty);

    virtual void trickWon(const Trick& trick, const Player* winner);

    virtual void abort();






protected slots:
    void cardSwapped(Card& c);
    void kittyButtonClicked();
    void bidMade(Human*, Bid bid);
    void cardTriedToPlay(Card& c);

signals:
    void kittyChosen(const Hand&);
    void acknowledgeTrick(Player* self);

    void updateScene();
    void requestBidDialog(Human*,Bidding*);
    void requestKittyButtonVisible(bool);
    void requestKittyButtonEnabled(bool);

protected:



    Suit m_trumps;

    const Trick* m_trick;

    Bid m_current_bid;
    Hand m_kitty_discards;
    Card* m_card_to_play;

    QMutex m_mutex;
    QWaitCondition m_wait;

    //int m_played_card_zindex;
};


#endif // HUMAN_H
