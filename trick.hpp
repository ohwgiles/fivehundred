#ifndef TRICK_H
#define TRICK_H
/*!
    \file trick.cpp
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
#include "suit.hpp"
#include "seat.hpp"

// Forward Declarations
class Player;
class Card;

/*!
  \class Trick
  \brief Maintains a list of (Player, Card) pairs

         Calls to playCard will add (Player, Card) pairs to the trick.

         The trick object can then determine a winner for any given trump suit
*/
class Trick {
public:
    Trick();

    struct Item {
        Player* player;
        Card* card;
    };

    void playCard(Player* player, Card* card); //!< Adds a player, card pair to the trick
    Player* winner(Suit trumps) const; //!< Returns the winner of a trick given a trump suit

    bool allPlayed() const { return m_num_cards == 4; } //!< True if four players have played
    int size() const { return m_num_cards; } //!< Returns the number of cards played in this trick

    Player* player(int index) const; //!< Returns the player at the given index in the trick
    Card* card(int index) const; //!< Returns the card at the given index in the trick

    std::vector<Card*> cards();

    /*!
      \class Comparator
      \brief Contains the logic for determining the value of a card in the trick

             This is a functor that takes the lead card suit and the trump suit
             in order to determine the relative value of two cards. In this way
             a player may determine which of two cards is better to play using
             std::max, or from a larger set using std::max_element
    */
    struct Comparator {
        bool operator()(const Card* lhs, const Card* rhs) const;
        bool operator()(const Item& lhs, const Item& rhs) const {
            return this->operator ()(lhs.card, rhs.card);
        }
        Suit trumps;
        Suit lead;
    };

protected:
    int m_num_cards;
    std::vector<Item> m_plays;
};

#endif // TRICK_H
