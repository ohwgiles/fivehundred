#ifndef DECK_H
#define DECK_H

#include <vector>
#include "card.hpp"

class Deck : public std::vector<Card*> {
public:
    Deck();
    virtual ~Deck();

    typedef std::vector<Card*> Cards;

    void deal(Cards& playerOne, Cards& playerTwo, Cards& playerThree, Cards& playerFour, Cards& kitty);

    void reposition(QSize screen);
    void hide();

private:
    char __cardspace[43*sizeof(Card)];
};

#endif // DECK_H
