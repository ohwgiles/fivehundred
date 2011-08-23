#include "human.hpp"
/*!
  \file human.cpp
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
#include "suit.hpp"
#include "trick.hpp"
#include "bidding.hpp"
#include <algorithm>
#include "jokersuitdialog.hpp"
#include <QMessageBox>
#include "log.hpp"

Human::Human(Seat pos, const QString& name) :
        Player(pos, name),
        m_trick(0),
        m_card_to_play(0)
{
    trace;
}

Human::~Human() {
    trace;
}

Bid Human::yourTurnToBid(const Bidding* bidlist) {
    trace;
    // Ask for the bid dialog, and block until the user selected something
    emit requestBidDialog(this, const_cast<Bidding*>(bidlist));
    m_wait.wait(&m_mutex);
    return m_current_bid;
}

void Human::abort() {
    trace;
    // WARNING this may cause the worker thread to return invalid or
    // uninitialised data, make sure you don't use it!
    m_wait.wakeAll();
}

void Human::sortHand(Suit trumps) {
    trace;
    // Define the sorter function: Spades < Diamonds < Clubs < Hearts
    auto sorter = [trumps](const Card* lhs, const Card* rhs) -> bool {
        trace;
        if(lhs->suit(trumps) == rhs->suit(trumps))
            return lhs->value(trumps) < rhs->value(trumps);

        else {
            if(lhs->suit(trumps) == Suit::SPADES)
                return true;
            else if(lhs->suit(trumps) == Suit::DIAMONDS)
                return rhs->suit(trumps) != Suit::SPADES;
            else if(lhs->suit(trumps) ==  Suit::CLUBS)
                return rhs->suit(trumps) == Suit::HEARTS || rhs->suit(trumps) == Suit::NONE;
            else if(lhs->suit(trumps) ==  Suit::HEARTS)
                return rhs->suit(trumps) == Suit::NONE;
            else return false;
        }
    };
    std::sort(hand.begin(), hand.end(), sorter);
}

Hand Human::yourTurnToSelectKitty(Hand& kitty) {
    debug;
    m_kitty_discards.clear();

    // Add the kitty to your hand and set them raised
    for(Hand::const_iterator c = kitty.begin(); c != kitty.end(); ++c) {
        hand.push_back(*c);
        (*c)->raise();
        //(*c)->setFaceUp(true);
        emit turnUpCard(*c, true);
    }
    for(Card* c: hand)
        connect(c, SIGNAL(cardClicked(Card&)), this, SLOT(cardSwapped(Card&)));

    sortHand(m_trumps);
    layoutHand();

    emit updateScene();

    // Make available the discard kitty button and wait for user to select cards
    emit requestKittyButtonVisible(true);
    m_wait.wait(&m_mutex);

    // m_kitty_discards should have been set by now, remove them from my hand
    for(Card* c: m_kitty_discards)
        hand.erase(std::find(hand.begin(), hand.end(), c));

    layoutHand();

    return m_kitty_discards;
}

Card* Human::yourTurnToPlay(const Trick* trick) {
    trace;
    // save the trick, we need it to determine whether the user is allowed to play a card
    m_trick = trick;
    for(Hand::iterator c = hand.begin(); c != hand.end(); ++c) {
        connect(*c, SIGNAL(cardClicked(Card&)), this, SLOT(cardTriedToPlay(Card&)));
    }
    m_wait.wait(&m_mutex);
    return m_card_to_play;
}

void Human::bidWon(const Bidding* bidlist, const Player*) {
    trace;
    m_trumps = bidlist->winner().bid.suit();
    sortHand(m_trumps);
    layoutHand();
}

void Human::cardSwapped(Card& c) {
    trace;
    unsigned i;
    for(i=0;i<hand.size(); ++i)
        if(hand[i] == &c) break;
    if(i == hand.size())
        fatal(error<<"Could not find clicked card in hand");
    Card* card = const_cast<Card*>(hand[i]);
    c.raise(!c.raised());
    emit placeCard(card, pos(), i, hand.size());
    //emit updateScene();
    // Count the number of raised cards
    unsigned numRaised = std::count_if(hand.begin(), hand.end(), [](Card*c){return c->raised();});
    emit requestKittyButtonEnabled(numRaised == 3);
}

void Human::bidMade(Human* player, Bid bid) {
    debug;
    if(player == this) {
        m_current_bid = bid;
        m_wait.wakeAll();
    }
}

void Human::handDealt() {
    sortHand(Suit::NONE);
    for(Card*c: hand)
        //c->setFaceUp();
        emit turnUpCard(c, true);
    layoutHand();
}

void Human::kittyButtonClicked() {
    trace;
    for(Hand::iterator it=hand.begin(); it!=hand.end(); ++it) {
        disconnect(*it, SIGNAL(cardClicked(Card&)), this, SLOT(cardSwapped(Card&)));
        if((*it)->raised()) {
            m_kitty_discards.push_back(*it);
        }
    }
    m_wait.wakeAll();
    emit requestKittyButtonVisible(false);
}

void Human::cardTriedToPlay(Card& card) {
    trace;
    if(m_trumps == Suit::NONE && card.value() == Card::JOKER) {
        if(m_trick->size() == 0) {
            JokerSuitDialog jd(this);
            if(jd.exec() == QDialog::Rejected)
                return;
            card.setJokerSuit(jd.chosenSuit());
        } else {
            if(offsuitPlayed.has(m_trick->card(0)->suit(m_trumps))) {
                std::stringstream q;
                q << "You've already played off suit on " << m_trick->card(0)->suit(m_trumps) <<
                            ". If you play the joker it will be as an off-suit card and hence of no value. Are you sure?";
                if(QMessageBox::question(0, "Joker", QString::fromStdString(q.str()), QMessageBox::Yes, QMessageBox::No) == QMessageBox::No) {
                    debug << "Bailing out of playing joker as offsuit";
                    return;
                }
                card.setJokerSuit(Suit::NONE);
            } else
                card.setJokerSuit(m_trick->card(0)->suit(m_trumps));
        }
    }
    debug << "I tried to play " << card;
    if(cardValid(m_trick, m_trumps, card)) {
        for(Hand::iterator c = hand.begin(); c != hand.end(); ++c) {
            disconnect(*c, SIGNAL(cardClicked(Card&)), this, SLOT(cardTriedToPlay(Card&)));
        }
        Hand::iterator c = std::find(hand.begin(), hand.end(), &card);
        if(c == hand.end())
            fatal(error<<"Somehow tried to play a card not in your hand");
        m_card_to_play = *c;
        hand.erase(c);
        m_wait.wakeAll();
    } else {
        // todo set status bar or something
        debug << "card not valid: " << card;
    }

}

void Human::trickWon(const Trick&, const Player*) {
    trace;
}
