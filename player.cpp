#include "player.hpp"
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
#include <QGraphicsScene>
#include <algorithm>
#include "card.hpp"
#include "trick.hpp"
#include "log.hpp"


//#define CARD_H_OFFSET 20
//#define CARD_EDGE_OFFSET 100

const char Player::className[] = "Player";

Player::Player(Seat pos, const QString& name) :
    QObject(),
    name(name),
    m_player_ui_label(name),
    next(0),
    m_pos(pos)
{
    trace;
}

bool Player::cardValid(const Trick* trick, Suit trumps, Card& card) {
    trace;
    if (trick->size() == 0) {
        debug << "This is the first card of the trick";
        return true; // No restrictions on the first card played
    }

    Card* leadCard = trick->card(0);

    if (leadCard->suit(trumps) == card.suit(trumps)) {
        debug << name << " followed suit";
        return true;
    }

    // At this point they have not played what was lead, check to ensure they are short suited.
    for(const Card* c: hand) {
        if (c->suit(trumps) == leadCard->suit(trumps)) {
            debug << name << " tried to play off suit but not short suited";
            return false; // They are not short suited, isTrump here handles the right / left bower
        }
    }

    debug << "Played off suit as short suited";
    return true;
}

void Player::layoutHand() {
    trace;
    for(unsigned i=0; i< hand.size(); ++i)
        hand[i]->setHandOffset(i, hand.size());
}


std::istream& operator>>(std::istream& s, Player& ) {
    trace;
    fatal(error<<"not implemented");
    char buffer[20];
    s.getline(buffer, 20, ':');
    //player.name = QString(buffer);
    for(int i=0; i< 10; ++i) {
        //s >> *player.hand[i];
    }
    return s;
}

std::ostream& operator<<(std::ostream& s, const Player& player) {
    trace;
    s << "Player \"" << player.name << "\"";
    return s;
}
