#include "trick.hpp"
#include <algorithm>
#include "card.hpp"
#include "player.hpp"
#include <iostream>
#include "log.hpp"




Trick::Trick() :
        m_num_cards(0),
        m_plays(4)
{
    trace;
}

Player* Trick::player(int index) const {
	if(!(index < m_num_cards))
	fatal(error<<"Index out of bounds");
	return m_plays[index].player;
}

Card* Trick::card(int index) const {
	if(!(index < m_num_cards))
		fatal(error<<"Index out of bounds");
		return m_plays[index].card;
}

void Trick::playCard(Player *player, Card *card) {
    trace;
    m_plays.at(m_num_cards).player = player;
    m_plays.at(m_num_cards).card = card;
    m_num_cards++;
}

bool Trick::CompareInTrick::operator ()(const Item& lhs, const Item& rhs) const {
    trace;
    return operator()(lhs.card, rhs.card);
}

bool Trick::CompareInTrick::operator ()(const Card* lhs, const Card* rhs) const {
    trace;
    if(lhs->suit(m_trumps) == rhs->suit(m_trumps))
        return lhs->value(m_trumps) < rhs->value(m_trumps);
    else if(lhs->suit(m_trumps) == m_trumps)
        return false;
    else if(rhs->suit(m_trumps) == m_trumps)
        return true;
    else
        return rhs->suit(m_trumps) == m_lead;
    error << "don't know how to compare " << lhs << " and " << rhs;
    return true;
}

Player* Trick::winner(Suit trumps) const {
    trace;
    CompareInTrick compare(trumps, m_plays.at(0).card->suit(trumps));
    return std::max_element(m_plays.begin(), m_plays.end(), compare)->player;
    //return winner->player;
}

std::vector<Card*> Trick::cards() {
    std::vector<Card*> cards;
    for(Item& i : m_plays) cards.push_back(i.card);
    return cards;
}


