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

Trick::Trick() :
        m_num_cards(0),
        m_plays(4)
{
    trace;
}

Player* Trick::player(int index) const {
    trace;
    if(!(index < m_num_cards))
        fatal(error<<"Index out of bounds");
    return m_plays[index].player;
}

Card* Trick::card(int index) const {
    trace;
    if(!(index < m_num_cards))
        fatal(error<<"Index out of bounds");
    return m_plays[index].card;
}

void Trick::playCard(Player *player, Card *card) {
    trace;
    // Append pair to list
    m_plays.at(m_num_cards).player = player;
    m_plays.at(m_num_cards).card = card;
    m_num_cards++;
}

bool Trick::Comparator::operator ()(const Card* lhs, const Card* rhs) const {
    trace;
    if(lhs->suit(trumps) == rhs->suit(trumps))
        // Same suit, just compare value
        return lhs->value(trumps) < rhs->value(trumps);
    // If one is trumps, it is bigger
    else if(lhs->suit(trumps) == trumps)
        return false;
    else if(rhs->suit(trumps) == trumps)
        return true;
    // Otherwise, we only care which card was the same as the lead card
    else
        return rhs->suit(trumps) == lead;

    // It should be impossible to end up here
    fatal(error << "don't know how to compare " << lhs << " and " << rhs);
}

Player* Trick::winner(Suit trumps) const {
    trace;
    // Use the Comparator functor to get the best card in the trick
    Comparator compare = {trumps, m_plays.at(0).card->suit(trumps)};
    return std::max_element(m_plays.begin(), m_plays.end(), compare)->player;
}

std::vector<Card*> Trick::cards() {
    trace;
    std::vector<Card*> cards;
    for(Item& i : m_plays)
        cards.push_back(i.card);
    return cards;
}


