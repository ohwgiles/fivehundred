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
    FOR_ALL_PLAYERS0(offsuitPlayed.reset)
    //emit sceneUpdated();
    info << "Starting the bidding";

    do {
        debug << "Next bidder is " << m_current_player->name();
        Bid bid = m_current_player->yourTurnToBid(&m_bids);
        if(m_abort) return ABORT;
        info << m_current_player->name() << " bid " << bid;
        if(receivers(SIGNAL(animateMakeBid(Seat,Bid))) > 0) {
            emit animateMakeBid(m_current_player->pos(), bid);
            m_wait.wait(&m_mutex);
        }
        debug;
        m_bids.bid(m_current_player, bid);
        debug;
    } while(!m_bids.complete() && (m_current_player = m_bids.nextBidder(m_current_player)));

    info << "Ended bidding process";

    if(receivers(SIGNAL(animateEndBidding())) > 0) {
        emit animateEndBidding();
        m_wait.wait(&m_mutex);
    }

    if(!m_bids.hasWinner())
        return NOBIDS;

    const Bidding::Pair& p = m_bids.winner();
    Player* unlucky_player = 0;
    int num_cards_in_trick = 4;
    if(p.bid.type() == Bid::CLOSED_MISERE || p.bid.type() == Bid::OPEN_MISERE) {
        // Bypass the bidder's partner, save it so we can restore it later
        unlucky_player = p.player->next->next;
        info << unlucky_player->name() << " will not play this round";
        p.player->next->next = unlucky_player->next;
        num_cards_in_trick = 3;
        if(p.bid.type() == Bid::OPEN_MISERE) {
            for(Card* c: p.player->hand) emit turnUpCard(c, true);
        }
    }
    m_current_player = p.player;
    m_trumps = p.bid.suit();
    std::stringstream ss;
    ss << p.bid << " by " << p.player->name();
    info << "Winning bid is " << ss.str();
    emit(updateBid(ss.str().c_str()));
    debug;
    FOR_ALL_PLAYERS2(bidWon, &m_bids, m_current_player);
    debug;
    for(Card* c : m_kitty)
        emit showCard(c, true);
    debug;
    Hand discards = m_current_player->yourTurnToSelectKitty(m_kitty);
    debug;
    if(m_abort) return ABORT;

    debug << m_current_player->name() << " discarded " << discards;

    for(Card* card : discards)
        emit showCard(card, false);

    while(m_tricks.size() < 10) {
        Trick* trick = new Trick();
        // store for posterity
        m_tricks.push_back(trick);

        do {
            emit updateState(m_current_player->name() + "'s turn");

            Card* c = m_current_player->yourTurnToPlay(trick);
            if(m_abort) return ABORT;
            emit turnUpCard(c, true);

            info << m_current_player->name() << " played " << *c;

            trick->playCard(m_current_player, c);
            // keep track of each player playing offsuit. This is a necessary evil to determine validity of playing joker in no trumps
            if(c->suit(m_trumps) != trick->card(0)->suit(m_trumps))
                m_current_player->offsuitPlayed.set(trick->card(0)->suit(m_trumps));
            if(receivers(SIGNAL(animatePlayCard(Seat,Card*,uint))) > 0) {
                m_current_player->reposition();
                emit animatePlayCard(m_current_player->pos(), c, trick->size());
                m_wait.wait(&m_mutex);
            }

            m_current_player = m_current_player->next;
        } while(trick->size() != num_cards_in_trick);

        Player* winner = trick->winner(m_trumps);
        info << winner->name() << " won the trick";
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

    if(unlucky_player) {
        // restore the natural order of things
        info << "Returning " << unlucky_player->name() << " to the game";
        unlucky_player->next->next->next->next = unlucky_player;
    }

    return SUCCESS;

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

int Contract::teamScoreResult(Player* player) const {
    const Bidding::Pair& p = bidWinner();
    Player* partner = player->next->next;
    if(p.bid.type() == Bid::CLOSED_MISERE || p.bid.type() == Bid::OPEN_MISERE) {
        if(p.player == player || p.player == partner)
            return tricksWon(player) + tricksWon(partner) == 0 ? p.bid.worth() : -p.bid.worth();
        else
            return 0;
    } else {
        if(p.player == player || p.player == partner)
            return tricksWon(player) + tricksWon(partner) >= p.bid.tricks() ? p.bid.worth() : -p.bid.worth();
        else
            return 10 * (tricksWon(player) + tricksWon(partner));
    }
}

int Contract::tricksWon(const Player* player) const {
    trace;
    return std::count_if(m_tricks.begin(), m_tricks.end(), [&](const Trick* t) {
        return t->winner(m_trumps) == player;
    });
}
