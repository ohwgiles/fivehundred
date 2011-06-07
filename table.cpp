#include "table.hpp"
#include "deck.hpp"
#include <QPushButton>
#include "os.hpp"
#include <QGraphicsProxyWidget>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include "log.hpp"
#include "bubble.hpp"

Table::Table(QPushButton* kittybutton) :
    QGraphicsScene(),
    m_deck(0),
    m_kittybuttonproxy(0)
{
    Card::createBackPixmap();
    m_deck = new Deck();

    for(Card* c: *m_deck)
        addItem(c);

    for(uint i=0; i<4; ++i) {
        m_bid_bubbles[i] = new Bubble(Seat(i));
        addItem(m_bid_bubbles[i]);
    }

    m_kittybuttonproxy = addWidget(kittybutton);

    QFont font;
    font.setPointSize(10);
    font.setStyleHint(QFont::SansSerif);

    // Add player names to scene
    for(QGraphicsTextItem& n: m_player_names) {
        n.setFont(font);
        n.setDefaultTextColor(QColor(Qt::white));
        addItem(&n);
    }
}

Table::~Table() {
    delete m_deck;
    for(Bubble* b: m_bid_bubbles) delete b;
}

void Table::setPlayerName(Seat pos, QString name) {
    m_player_names[pos].setPlainText(name);
}


void Table::reposition(QSize size) {
    setSceneRect(0, 0, size.width(), size.height());

    m_kittybuttonproxy->setPos(size.width()/2 - m_kittybuttonproxy->rect().width()/2, size.height()/2 - m_kittybuttonproxy->rect().height()/2);

    m_player_names[int(SOUTH)].setPos(
                size.width()/2 +
                4.5*Card::H_OFFSET + Card::WIDTH/2 - m_player_names[int(SOUTH)].boundingRect().width(),
                size.height() - Card::EDGE_OFFSET + Card::HEIGHT/2);
    m_player_names[int(NORTH)].setPos(
                size.width()/2 -
                4.5*Card::H_OFFSET - Card::WIDTH/2,
                Card::EDGE_OFFSET - Card::HEIGHT/2 - m_player_names[int(NORTH)].boundingRect().height());
    m_player_names[int(WEST)].setPos(
                Card::EDGE_OFFSET - Card::HEIGHT/2,
                size.height()/2 + 4.5*Card::H_OFFSET + Card::WIDTH/2);
    m_player_names[int(EAST)].setPos(
                size.width() -
                Card::EDGE_OFFSET + Card::HEIGHT/2 - m_player_names[int(EAST)].boundingRect().width(),
                size.height()/2 - 4.5*Card::H_OFFSET - Card::WIDTH/2 -
                m_player_names[int(SOUTH)].boundingRect().height());

    const int edge = 170;
    QRectF extents(m_bid_bubbles[0]->boundingRect());
    m_bid_bubbles[SOUTH]->setPos(3*size.width()/5 - extents.width()/2, size.height() - extents.height()/2 - edge);
    m_bid_bubbles[WEST]->setPos(edge + extents.width()/2, 3*size.height()/5 - extents.height()/2);
    m_bid_bubbles[NORTH]->setPos(2*size.width()/5 + extents.width()/2, edge + extents.height()/2);
    m_bid_bubbles[EAST]->setPos(size.width() - edge - extents.width()/2, 2*size.height()/5 + extents.height()/2);
}

void Table::clear() {
    m_deck->show(false);
    m_kittybuttonproxy->hide();
    for(QGraphicsTextItem& n: m_player_names)
        n.setPlainText("");
    for(Bubble* b: m_bid_bubbles)
        b->setScale(0);
}

void Table::showCard(Card* card, bool en) {
    trace;
    if(en) {
        card->show();
    } else {
        card->hide();
        card->raise(false);
    }
    card->setOpacity(1);
}

void Table::animateBidBubble(Seat pos, Bid bid) {
    trace;
    std::stringstream ss;
    ss << bid;
    m_bid_bubbles[pos]->setOpacity(1);
    m_bid_bubbles[pos]->setText(QString::fromStdString(ss.str()));
    QPropertyAnimation* bubble = new QPropertyAnimation(m_bid_bubbles[pos], "scale");
    bubble->setDuration(450);
    bubble->setKeyValueAt(0, 0);
    bubble->setKeyValueAt(1, 1);
    bubble->setEasingCurve(QEasingCurve::OutBack);
    connect(bubble, SIGNAL(finished()), this, SLOT(animationComplete_()));
    bubble->start(QAbstractAnimation::DeleteWhenStopped);

}

void Table::hideBubbles() {
    trace;
    QParallelAnimationGroup* anim_group = new QParallelAnimationGroup();
    for(Bubble* b : m_bid_bubbles) {
        QPropertyAnimation* opacity = new QPropertyAnimation(b, "opacity");
        opacity->setDuration(500);
        opacity->setKeyValueAt(0, 1);
        opacity->setKeyValueAt(1, 0);
        anim_group->addAnimation(opacity);
    }
    connect(anim_group, SIGNAL(finished()), this, SLOT(animationComplete_()));
    anim_group->start(QAbstractAnimation::DeleteWhenStopped);
}

void Table::animatePlayCard(Seat origin, Card* card,uint stackOrder) {
    trace;
    QPointF final = cardPosition(origin);// card->expectedPosition(ui->graphicsView->size(), Card::TRICK);
    card->setZValue(stackOrder);
    QPropertyAnimation* anim = new QPropertyAnimation(card, "position");
    connect(anim, SIGNAL(finished()), this, SLOT(animationComplete_()));
    anim->setDuration(600);
    anim->setEasingCurve(QEasingCurve::OutCirc);
    anim->setKeyValueAt(0, card->pos());
    anim->setKeyValueAt(1, final);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

QPointF Table::cardPosition(Seat origin) {
    trace;
    QSizeF size(this->sceneRect().size());
    switch(origin) {
    case SOUTH:
        return QPointF(size.width()/2 + Card::H_OFFSET, size.height()/2 + Card::H_OFFSET);
    case WEST:
        return QPointF(size.width()/2 - Card::H_OFFSET, size.width()/2 + Card::H_OFFSET);
    case NORTH:
        return QPointF(size.width()/2 - Card::H_OFFSET, size.width()/2 - Card::H_OFFSET);
    case EAST:
        return QPointF(size.width()/2 + Card::H_OFFSET, size.width()/2 - Card::H_OFFSET);
    }
    fatal(error<<"Impossible code path");
}

void Table::animateCollectCards(std::vector<Card*> cards, Seat anc) {
    trace;
    QSizeF size(this->sceneRect().size());
    QParallelAnimationGroup* anim_group = new QParallelAnimationGroup();
    for(Card* c : cards) {
        QPropertyAnimation* pos = new QPropertyAnimation(c, "position");
        pos->setDuration(1000);
        pos->setKeyValueAt(0, c->pos());
        pos->setKeyValueAt(0.5, c->pos());
        QPointF final;
        switch(anc) {
        case SOUTH:
            final = QPointF(size.width()/2, size.height());
            break;
        case WEST:
            final = QPointF(0, size.height()/2);
            break;
        case EAST:
            final = QPointF(size.width(), size.height()/2);
            break;
        case NORTH:
            final = QPointF(size.width()/2, 0);
            break;
        default:
            fatal(error<<"Invalid anchor point");
        }

        pos->setKeyValueAt(1, final);
        anim_group->addAnimation(pos);

        QPropertyAnimation* opacity = new QPropertyAnimation(c, "opacity");
        opacity->setDuration(1000);
        opacity->setKeyValueAt(0, 1);
        opacity->setKeyValueAt(0.5, 1);
        opacity->setKeyValueAt(0.8, 0);
        opacity->setKeyValueAt(1, 0);
        anim_group->addAnimation(opacity);
    }
    connect(anim_group, SIGNAL(finished()), this, SLOT(animationComplete_()));
    anim_group->start(QAbstractAnimation::DeleteWhenStopped);
}

void Table::animationComplete_() {
    trace;
    emit animationComplete();
}


void Table::placeCardInHand(Card* card, Seat playerSeat, unsigned indexInHand, unsigned handSize) {
    trace;
    QSizeF size(sceneRect().size());
    double relative_pos = (indexInHand - handSize/2.0);
    switch(playerSeat) {
    case SOUTH:
        card->setPos(Card::H_OFFSET * relative_pos+size.width()/2 + Card::H_OFFSET/2, size.height() - Card::EDGE_OFFSET - (card->raised() ? Card::RAISE : 0));
        card->setRotation(0);
        break;
    case WEST:
        card->setPos(Card::EDGE_OFFSET + (card->raised() ? Card::RAISE : 0), size.height()/2 + Card::H_OFFSET*relative_pos + Card::H_OFFSET/2);
        card->setRotation(90);
        break;
    case NORTH:
        card->setPos(Card::H_OFFSET*(-relative_pos)+size.width()/2 - Card::H_OFFSET/2, Card::EDGE_OFFSET + (card->raised() ? Card::RAISE : 0));
        card->setRotation(0);
        break;
    case EAST:
        card->setPos(size.width()-Card::EDGE_OFFSET - (card->raised() ? Card::RAISE : 0), size.height()/2 + Card::H_OFFSET*(-relative_pos) - Card::H_OFFSET/2);
        card->setRotation(-90);
        break;
    }
    card->setZValue(indexInHand);
}

void Table::placeCardInTrick(Card *card, Seat orientation, unsigned trickIndex) {
    trace;
    card->setPos(cardPosition(orientation));
    card->setZValue(trickIndex);
}

void Table::turnUpCard(Card *card, bool faceUp) {
    trace;
    card->setFaceUp(faceUp);
}


