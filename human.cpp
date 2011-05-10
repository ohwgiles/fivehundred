#include <algorithm>
#include "human.hpp"

#include "card.hpp"
#include "suit.hpp"
#include "trick.hpp"
#include "bidding.hpp"
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
    emit requestBidDialog(this, const_cast<Bidding*>(bidlist));
    m_wait.wait(&m_mutex);
    return m_current_bid;
}

void Human::abort() {
    trace;
    m_wait.wakeAll();
}

Hand Human::yourTurnToSelectKitty(const Hand& kitty) {
    debug;
    for(Hand::const_iterator c = kitty.begin(); c != kitty.end(); ++c) {
        hand.push_back(*c);
        (*c)->raise();
        (*c)->setFaceUp(true);
    }
    Card::CompareInHand comparison(m_trumps);
    std::sort(hand.begin(), hand.end(), comparison);
    for(Card* c: hand)
        connect(c, SIGNAL(cardClicked(Card&)), this, SLOT(cardSwapped(Card&)));
    layoutHand();
    emit updateScene();

    emit requestKittyButtonVisible(true);
    m_wait.wait(&m_mutex);

    for(Card* c: m_kitty_discards)
        hand.erase(std::find(hand.begin(), hand.end(), c));
    layoutHand();

    return m_kitty_discards;
}

Card* Human::yourTurnToPlay(const Trick* trick) {
    trace;
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
    Card::CompareInHand comparison(m_trumps);
    std::sort(hand.begin(), hand.end(), comparison);
    layoutHand();
}

void Human::cardSwapped(Card& c) {
    trace;
    int xd = (c.raised()?-1:1)*(m_pos==WEST?1:-1)*(m_pos==EAST||m_pos==WEST?CARD_RAISE:0);
    int yd = (c.raised()?-1:1)*(m_pos==NORTH?1:-1)*((m_pos==NORTH||m_pos==SOUTH)?CARD_RAISE:0);
    c.raise(!c.raised());
    c.setPos(c.x() + xd, c.y() + yd);
    unsigned numRaised = 0;
    for(Hand::iterator it=hand.begin(); it!=hand.end(); (*it++)->raised() ? numRaised++ : 0);
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
    Card::CompareInHand comparison(Suit::NONE);
    std::sort(hand.begin(), hand.end(), comparison);
    for(Card*c: hand)
        c->setFaceUp();
    layoutHand();
}

void Human::kittyButtonClicked() {
    info;
    for(Hand::iterator it=hand.begin(); it!=hand.end(); ++it) {
		info << "in loop";
        disconnect(*it, SIGNAL(cardClicked(Card&)), this, SLOT(cardSwapped(Card&)));
        if((*it)->raised()) {
            m_kitty_discards.push_back(*it);
        }
    }
    info << "after work";
    m_wait.wakeAll();
    info << "after wake";
    emit requestKittyButtonVisible(false);
}

void Human::cardTriedToPlay(Card& card) {
    trace;
    debug << "I tried to play " << card;
    if(cardValid(m_trick, m_trumps, card)) {
        for(Hand::iterator c = hand.begin(); c != hand.end(); ++c) {
            disconnect(*c, SIGNAL(cardClicked(Card&)), this, SLOT(cardTriedToPlay(Card&)));
        }
        Hand::iterator c = std::find(hand.begin(), hand.end(), &card);
        assert(c != hand.end());
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
