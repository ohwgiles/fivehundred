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
#include "suit.hpp"
#include "bid.hpp"
#include "seat.hpp"
#include <vector>
#include <iosfwd>
#include <QObject>

// Forward Declarations
class Card;
class Trick;
class Bidding;

typedef std::vector<Card*> Hand;

/*!
  \class Player
  \brief Base class for Player types

  This class defines the interface for Players. Every player must implement
  the set of pure virtual functions defined here
*/
class Player : public QObject
{
    Q_OBJECT
public:
    //! Every Lua-accessible object must have a string identifier
    static const char className[];

    Player(Seat pos, const QString& name);

    virtual void configure() {} //!< Optional configure function, called after next Player pointers are set up

    Hand hand;

    virtual void abort() {} //!< Optional interface called from GUI thread to abort current processing
    virtual void handDealt() = 0; //!< Called when the player's hand has been populated
    virtual Bid yourTurnToBid(const Bidding* bidlist) = 0;
    virtual void bidWon(const Bidding* bidlist, const Player* winner) = 0;
    virtual Hand yourTurnToSelectKitty(const Hand& kitty) = 0;
    virtual Card* yourTurnToPlay(const Trick*) = 0;
    virtual void trickWon(const Trick& trick, const Player* winner) = 0;

    bool operator==(const Player& other) { return this == &other; }

    Seat pos() const { return m_pos; }

    const QString name;
    Player* next; //!< Visitor-pattern linked list behaviour

signals:
    void kittyChosen(const Hand&);
    void acknowledgeTrick(Player* self);

protected:
    //! Contains logic to determine whether the player is allowed to play, e.g. short suited
    bool cardValid(const Trick* trick, Suit trumps, Card& card);
    //! Set the positional offset of each card in the player's hand
    void layoutHand();
    Seat m_pos;
};


std::istream& operator>>(std::istream& s, Player& player);
std::ostream& operator<<(std::ostream& s, const Player& player);

#endif // PLAYER_HPP
