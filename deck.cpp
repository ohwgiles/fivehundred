#include "deck.hpp"
#include <algorithm>
#include <ctime>
#include "log.hpp"

#include <QGraphicsScene>

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

void Deck::hide() {
    trace;
    for(Card* c: *this) {
        c->setLocation(Card::HIDDEN);
        c->raise(false);
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
    for(Card* c: *this)
        c->setFaceUp(false);
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

void Deck::reposition(QSize screen) {
    for(Card* c: *this)
        c->reposition(screen);
}
