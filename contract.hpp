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
#include <vector>
#include <QObject>
#include <QGraphicsScene>
#include <QLabel>
#include <QMutex>
#include <QWaitCondition>

#include "player.hpp"
#include "trick.hpp"
#include "bidding.hpp"

class Player;

class Contract : public QObject {
    Q_OBJECT
public:
    Contract(Player* first_player);
    ~Contract();

    enum ExitState { SUCCESS, NOBIDS, REDEAL, ABORT };
    ExitState start();

    const Bidding::Pair& bidWinner() const { return m_bids.winner(); }
    bool successful() const;
    int tricksWon(const Player* player) const;

    std::vector<Card*> m_kitty;
    void abort();

signals:
    void sceneUpdated();
    void updateState(QString);
    void updateBid(QString);
    void updateEastWestTricks(QString);
    void updateNorthSouthTricks(QString);
    void animatePlayCard(Card*);
    void animateCollectCards(std::vector<Card*>, Seat);

protected slots:
    void wake();

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
