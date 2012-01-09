#ifndef TABLE_HPP
#define TABLE_HPP
/*!
  \file table.hpp
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

#include "seat.hpp"
#include "bid.hpp"
#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>

class Card;
class Deck;
class QPushButton;
class Bubble;

/*!
  \class Table
  \brief Graphical scene depicting a square of 4 players
*/
class Table : public QGraphicsScene {
    Q_OBJECT
public:
    Table(QPushButton* kittybutton);
    ~Table();
    Deck* deck() { return m_deck; }
    void setPlayerName(Seat pos, QString name);
    void reposition(QSize size);
    void clear();

protected:
    void repositionPlayerNames();
    QPointF cardPosition(Seat origin);

    Deck* m_deck;
    QGraphicsProxyWidget* m_kittybuttonproxy;

    Bubble* m_bid_bubbles[4];
    QGraphicsTextItem m_player_names[4];

signals:
    void animationComplete(); //!< Emitted when the GUI has finished animating an object

protected slots:
    void showCard(Card* card, bool en);
    void placeCardInHand(Card* card, Seat playerSeat, unsigned indexInHand, unsigned handSize); //!< Players can rearrange their cards
    void placeCardInTrick(Card* card, Seat orientation, unsigned trickIndex);
    void turnUpCard(Card* card, bool faceUp);
    void animateBidBubble(Seat,Bid);
    void hideBubbles();
    void animatePlayCard(Seat,Card*,uint stackOrder); //!< the Contract requests animations
    void animateCollectCards(std::vector<Card*>, Seat); //!< the Contract requests animations
    void animationComplete_(); //!< Internal callback when the animation is complete

};

#endif // TABLE_HPP
