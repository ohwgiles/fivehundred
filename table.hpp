#ifndef TABLE_HPP
#define TABLE_HPP

#include "seat.hpp"
#include "bid.hpp"
#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>

class Card;
class Deck;
class QPushButton;
class Bubble;

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
