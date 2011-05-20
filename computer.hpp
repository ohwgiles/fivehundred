#ifndef COMPUTER_HPP
#define COMPUTER_HPP
/*!
  \file computer.hpp
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

// Forward Declarations
struct lua_State;
struct LuaRegistration;

/*!
  \class Computer
  \brief Player controlled by a Lua script
*/
class Computer : public Player {
public:
    Computer(Seat pos, const QString& name, const QString& script, bool play_open_hand);
    virtual ~Computer();
    virtual void configure();
    virtual void handDealt();

    virtual Card* yourTurnToPlay(const Trick*);
    virtual Bid yourTurnToBid(const Bidding* bidlist);
    virtual void bidWon(const Bidding* bidlist, const Player* winner);
    virtual Hand yourTurnToSelectKitty(const Hand& kitty);

    virtual void trickWon(const Trick& trick, const Player* winner);

    Suit m_trumps;
    Suit m_lead;

protected:

    void pushBidList(const Bidding* bidlist);
    void registerHand();

    lua_State* L;
    LuaRegistration* luareg;

    Bidding* m_current_bidlist;
    Trick* m_current_trick;

    bool m_play_open_hand;
};

#endif // COMPUTER_HPP
