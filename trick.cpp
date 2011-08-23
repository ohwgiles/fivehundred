#include "trick.hpp"
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
#include "card.hpp"
#include "player.hpp"
#include <algorithm>
#include "log.hpp"

Trick::Trick()
{
    trace;
}

Player* Trick::player(int index) const {
    trace;
    if(index >= size())
        fatal(error<<"Index out of bounds");
    return m_plays[index].player;
}

Card* Trick::card(int index) const {
    trace;
    if(index >= size())
        fatal(error<<"Index out of bounds");
    return m_plays[index].card;
}

void Trick::playCard(Player *player, Card *card) {
    trace;
    if(size() == 0 && card->suit() == Suit::NONE)
        fatal(error<<"Lead card must have a suit. If it is the joker, set its suit before playing.");
    // Append pair to list
    m_plays.push_back( { player, card } );
}


bool Trick::Comparator::operator ()(const SimpleCard& lhs, const SimpleCard& rhs) const {
    trace;
    trace << "comparing " << lhs << " to " << rhs << " with trump suit " << trumps << " and lead suit " << lead;

    // If something has no suit (really can only be joker played as offsuit), it has no value
    if(lhs.suit(trumps) == Suit::NONE) return !noSuitJokerHigh;
    if(rhs.suit(trumps) == Suit::NONE) return noSuitJokerHigh;

    // Same suit, just compare value
    if(lhs.suit(trumps) == rhs.suit(trumps))
        return lhs.value(trumps) < rhs.value(trumps);

    // If one is trumps, it is bigger
    if(lhs.suit(trumps) == trumps)
        return false;
    if(rhs.suit(trumps) == trumps)
        return true;

    // Otherwise, if one was the lead card, it's bigger
    if(lhs.suit(trumps) == lead)
        return false;
    if(rhs.suit(trumps) == lead)
        return true;

    // Otherwise, just compare on value (all other suits are equal)
    return lhs.value() < rhs.value();
}

Player* Trick::winner(Suit trumps) const {
    trace;
    // Use the Comparator functor to get the best card in the trick
    Comparator compare(trumps, m_plays.at(0).card->suit(trumps), false);
    return std::max_element(m_plays.begin(), m_plays.end(), compare)->player;
}

std::vector<Card*> Trick::cards() {
    trace;
    std::vector<Card*> cards;
    for(Item& i : m_plays)
        cards.push_back(i.card);
    return cards;
}


