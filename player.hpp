#ifndef PLAYER_HPP
#define PLAYER_HPP
/*!
  \file player.cpp
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
