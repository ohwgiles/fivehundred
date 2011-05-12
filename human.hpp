#ifndef HUMAN_H
#define HUMAN_H
/*!
  \file human.hpp
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

#include "player.hpp"
#include <QObject>
#include <QWaitCondition>
#include <QMutex>

// Forward Declarations
class Bid;

/*!
  \class Human
  \brief A human player

  Implements all the pure virtual methods of Player, emitting a signal
  to another thread and locking a mutex when it expects user input, i.e.
  a Card to play or a Bid to select.

  It should be fairly trivial to connect the signals of this class to
  the slots of another class to provide an alternative user interface.

  \see MainWindow::newGame
*/
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

    //! Wake all current mutexes. Invalid data may be returned, so make sure you don't use it!
    virtual void abort();

protected slots:
    void cardSwapped(Card& c);
    void kittyButtonClicked();
    void bidMade(Human*, Bid bid);
    void cardTriedToPlay(Card& c);

signals:
    void updateScene();
    void requestBidDialog(Human*,Bidding*);
    void requestKittyButtonVisible(bool);
    void requestKittyButtonEnabled(bool);

protected:
    void sortHand(Suit trumps); //!< Rearrange cards in aesthetically pleasing order

    Suit m_trumps;
    const Trick* m_trick;

    Bid m_current_bid;
    Hand m_kitty_discards;
    Card* m_card_to_play;

    QMutex m_mutex;
    QWaitCondition m_wait;
};


#endif // HUMAN_H
