#include "contract.hpp"
/*!
  \file contract.cpp
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
#include "card.hpp"
#include <algorithm>
#include <sstream>
#include "log.hpp"


Contract::Contract(Player* first_player) :
    QObject(),
    m_abort(false),
    m_current_player(first_player),
    m_northsouth_tricks(0),
    m_eastwest_tricks(0)
{
    trace;
}

Contract::~Contract() {
    trace;
    for(Trick* t: m_tricks)
        delete t;
}

#define FOR_ALL_PLAYERS0(method) \
    m_current_player->method(); \
    m_current_player->next->method(); \
    m_current_player->next->next->method(); \
    m_current_player->next->next->next->method();
#define FOR_ALL_PLAYERS2(method, arg1, arg2) \
    m_current_player->method(arg1, arg2); \
    m_current_player->next->method(arg1, arg2); \
    m_current_player->next->next->method(arg1, arg2); \
    m_current_player->next->next->next->method(arg1, arg2);

Contract::ExitState Contract::start() {
    trace;
    for(Card* c : m_kitty)
        emit showCard(c, false);

    emit(updateNorthSouthTricks("0"));
    emit(updateEastWestTricks("0"));
    emit updateBid("");

    FOR_ALL_PLAYERS0(handDealt);
    //emit sceneUpdated();
    info << "Starting the bidding";

    do {
        debug << "Next bidder is " << m_current_player->name;
        Bid bid = m_current_player->yourTurnToBid(&m_bids);
        if(m_abort) return ABORT;
        info << m_current_player->name << " bid " << bid;
        if(receivers(SIGNAL(animateMakeBid(Seat,Bid))) > 0) {
            emit animateMakeBid(m_current_player->pos(), bid);
            m_wait.wait(&m_mutex);
        }
        m_bids.bid(m_current_player, bid);
    } while(!m_bids.complete() && (m_current_player = m_bids.nextBidder(m_current_player)));

    info << "Ended bidding process";

    if(receivers(SIGNAL(animateEndBidding())) > 0) {
        emit animateEndBidding();
        m_wait.wait(&m_mutex);
    }

    if(m_bids.hasWinner()) {
        const Bidding::Pair& p = m_bids.winner();
        m_current_player = p.player;
        m_trumps = p.bid.suit();
        std::stringstream ss;
        ss << p.bid << " by " << p.player->name;
        info << "Winning bid is " << ss.str();
        emit(updateBid(ss.str().c_str()));
        FOR_ALL_PLAYERS2(bidWon, &m_bids, m_current_player);

        for(Card* c : m_kitty)
            emit showCard(c, true);

        Hand discards = m_current_player->yourTurnToSelectKitty(m_kitty);
        if(m_abort) return ABORT;

        debug << m_current_player->name << " discarded " << discards;

        for(Card* card : discards)
            emit showCard(card, false);

        while(m_tricks.size() < 10) {
            Trick* trick = new Trick();
            // store for posterity
            m_tricks.push_back(trick);

            do {
                emit updateState(m_current_player->name + "'s turn");

                Card* c = m_current_player->yourTurnToPlay(trick);
                if(m_abort) return ABORT;
                emit turnUpCard(c, true);

                info << m_current_player->name << " played " << *c;

                trick->playCard(m_current_player, c);
                if(receivers(SIGNAL(animatePlayCard(Seat,Card*,uint))) > 0) {
                    m_current_player->reposition();
                    emit animatePlayCard(m_current_player->pos(), c, trick->size());
                    m_wait.wait(&m_mutex);
                }

                m_current_player = m_current_player->next;
            } while(!trick->allPlayed());

            Player* winner = trick->winner(m_trumps);
            info << winner->name << " won the trick";
            if(winner->pos() == EAST || winner->pos() == WEST)
                emit(updateEastWestTricks( QString::number(++m_eastwest_tricks)));
            else
                emit(updateNorthSouthTricks(QString::number(++m_northsouth_tricks)));

            FOR_ALL_PLAYERS2(trickWon, *trick, winner);

            std::vector<Card*> discards = trick->cards();
            if(receivers(SIGNAL(animateCollectCards(std::vector<Card*>,Seat))) > 0) {
                emit animateCollectCards(discards, winner->pos());
                m_wait.wait(&m_mutex);
            }
            for(Card* c: discards)
                emit showCard(c, false);

            m_current_player = winner;
        }
        return SUCCESS;
    } else {
        return NOBIDS;
    }

}

void Contract::wake() {
    trace;
    m_wait.wakeAll();
}

void Contract::abort() {
    info;
    m_abort = true;
}

void Contract::reposition() {
    trace;
    if(!m_tricks.empty())
        for(int i=0; i<m_tricks.back()->size(); ++i) {
            emit placeCard(m_tricks.back()->card(i), m_tricks.back()->player(i)->pos(), i);
        }
}

bool Contract::successful() const {
    trace;
    const Bidding::Pair& p = bidWinner();
    return (tricksWon(p.player) + tricksWon(p.player->next->next)) >= p.bid.tricks();
}

int Contract::tricksWon(const Player* player) const {
    trace;
    return std::count_if(m_tricks.begin(), m_tricks.end(), [&](const Trick* t) {
        return t->winner(m_trumps) == player;
    });
}
