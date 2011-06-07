#include "deck.hpp"
/*!
  \file deck.cpp
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
#include <algorithm>
#include "log.hpp"
#include <ctime>

Deck::Deck() {
    trace;
    int i;
    int j = 0;

    for(i=Card::FOUR; i <= Card::ACE; ++i) {
        new(&__cardspace[j]) Card(Suit::DIAMONDS, (Card::Value)i);
        j+=sizeof(Card);
        new(&__cardspace[j]) Card(Suit::HEARTS, (Card::Value)i);
        j+=sizeof(Card);
    }
    for(i=Card::FIVE; i <= Card::ACE; ++i) {
        new(&__cardspace[j]) Card(Suit::SPADES, (Card::Value)i);
        j+=sizeof(Card);
        new(&__cardspace[j]) Card(Suit::CLUBS, (Card::Value)i);
        j+=sizeof(Card);
    }
    new(&__cardspace[j]) Card(Suit::NONE, Card::JOKER);

    resize(43);
    for(unsigned i=0; i < 43; ++i) at(i) = (Card*)&__cardspace[i*sizeof(Card)];

}

Deck::~Deck() {
    trace;
    for(Card* c: *this)
        c->~Card();
}

void Deck::show(bool en) {
    trace;
    for(Card* c: *this) {
//        c->setLocation(Card::HIDDEN);
//        c->raise(false);
        if(en) c->show(); else c->hide();
    }
}

void Deck::deal(Cards& playerOne, Cards& playerTwo, Cards& playerThree, Cards& playerFour, Cards& kitty) {
    trace;
    srand(unsigned(time(0)));
    std::random_shuffle(begin(), end());
    std::vector<Card*>::iterator it = begin();
    playerOne.clear();
    playerTwo.clear();
    playerThree.clear();
    playerFour.clear();
    for(unsigned i = 0; i < 10; ++i) {
        playerOne.push_back(*it++);
        playerTwo.push_back(*it++);
        playerThree.push_back(*it++);
        playerFour.push_back(*it++);
    }
    kitty.clear();
    kitty.push_back(*it++);
    kitty.push_back(*it++);
    kitty.push_back(*it);
}

