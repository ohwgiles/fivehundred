#ifndef DECK_H
#define DECK_H
/*!
  \file deck.hpp
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
#include "card.hpp"

class Deck : public std::vector<Card*> {
public:
    Deck();
    virtual ~Deck();

    typedef std::vector<Card*> Cards;

    //! Shuffle all cards and deal them out
    void deal(Cards& playerOne, Cards& playerTwo, Cards& playerThree, Cards& playerFour, Cards& kitty);

    void reposition(QSize screen);
    void hide();

private:
    char __cardspace[43*sizeof(Card)];
};

#endif // DECK_H
