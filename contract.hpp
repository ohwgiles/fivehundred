#ifndef CONTRACT_HPP
#define CONTRACT_HPP
/*!
  \file contract.hpp
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
#include "trick.hpp"
#include "bidding.hpp"

#include <QGraphicsScene>
#include <QLabel>
#include <QMutex>
#include <QWaitCondition>
#include <QObject>
#include <vector>

// Forward Declarations
class Player;

/*!
  \class Contract
  \brief The interesting part of the game

  This class controls the core behaviour of the entire game. It controls
  the bidding process, swapping the kitty and managing tricks
*/
class Contract : public QObject {
    Q_OBJECT
public:
    Contract(Player* first_player);
    ~Contract();

    enum ExitState { SUCCESS, NOBIDS, REDEAL, ABORT }; //!< Return values for any given run of a contract
    ExitState start(); //!< Starts running the contract

    //! Return the Player, Bid pair which won the bid process
    const Bidding::Pair& bidWinner() const { return m_bids.winner(); }

    //! Whether or not the winning bidder achieved his bid
    bool successful() const;

    //! Return the number of tricks won by the given player
    int tricksWon(const Player* player) const;

    void abort();
    std::vector<Card*> m_kitty;

signals:
    void sceneUpdated();
    void updateState(QString);
    void updateBid(QString);
    void updateEastWestTricks(QString);
    void updateNorthSouthTricks(QString);
    void animatePlayCard(Card*);
    void animateCollectCards(std::vector<Card*>, Seat);

protected slots:
    void wake(); //!< Asynchronous wake from from mutex

protected:
    bool m_abort;
    std::vector<Trick*> m_tricks;
    Suit m_trumps;
    Bidding m_bids;
    Player* m_current_player;
    QMutex m_mutex;
    QWaitCondition m_wait;
    int m_northsouth_tricks;
    int m_eastwest_tricks;
};

#endif // CONTRACT_HPP
