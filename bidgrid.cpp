#include <QPainter>
#include <QMouseEvent>
#include "bid.hpp"
#include "os.hpp"
#include "bidgrid.hpp"
#include "log.hpp"

BidGrid::BidGrid(QWidget *parent) :
    QWidget(parent),
    m_mouse_square(POS_NONE),
    m_selected_square(POS_NONE),
    m_disabled_threshold(0),
    m_grid_image(os::GFX_PATH + "bidgrid.png")
{
    this->setMouseTracking(true);
    //resize(m_grid_image.width(), m_grid_image.height());
    this->setMinimumSize(m_grid_image.size());
    this->setMaximumSize(m_grid_image.size());
}

void BidGrid::drawRect(QPainter& painter, uint i, uint j) {
    if(coordToIndex(i,j) <= m_disabled_threshold)
        painter.setBrush(QBrush(QColor(0xad, 0xa0, 0xa0)));
    else if(coordToIndex(i,j) == m_selected_square)
        painter.setBrush(QBrush(QColor(0xc4, 0xff, 0xc4)));
    else if(coordToIndex(i,j) == m_mouse_square)
        painter.setBrush(QBrush(QColor(0xc4, 0xe2, 0xff)));
    else
        painter.setBrush(QBrush(QColor(0xff, 0xff, 0xee)));
    painter.drawRect(X_OFFSET + i*X_SIZE, Y_OFFSET+j*Y_SIZE, X_SIZE, Y_SIZE);
}

void BidGrid::setWinningBid(const Bid& bid) {
    int result = m_disabled_threshold;
    switch(bid.type()) {
    case Bid::PASS:
        result = POS_NONE;
        break;
    case Bid::CLOSED_MISERE:
        result = coordToIndex(5, 1);
        break;
    case Bid::OPEN_MISERE:
        result = coordToIndex(5, 3);
        break;
    case Bid::NORMAL:
        result = coordToIndex(Bid::suitToInt(bid.suit()), bid.tricks()-6);
    }
    if(result != m_disabled_threshold) {
        m_disabled_threshold = result;
        update();
    }
}

void BidGrid::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setPen(Qt::NoPen);

    for(uint i=0; i<5; ++i) {
        for(uint j=0; j<5; ++j) {
            drawRect(painter, i, j);
        }
    }
    drawRect(painter, 5, 1);
    drawRect(painter, 5, 3);
    painter.drawPixmap(0, 0, m_grid_image);
}

void BidGrid::mouseMoveEvent(QMouseEvent* event) {
    int result = mousePosToIndex(event);
    if(result != m_mouse_square) {
        m_mouse_square = result;
        this->update();
    }
}

int BidGrid::mousePosToIndex(QMouseEvent* event) {
    if(uint(event->x()) > X_OFFSET && uint(event->y()) > Y_OFFSET) {
        uint x = (event->x() - X_OFFSET) / X_SIZE;
        uint y = (event->y() - Y_OFFSET) / Y_SIZE;
        if(x == 5 && y != 1 && y !=3)
            return POS_NONE;
        else
            return coordToIndex(x,y);
    } else return POS_NONE;
}

void BidGrid::mousePressEvent(QMouseEvent* event) {
    int result = mousePosToIndex(event);
    if(result != m_selected_square) {
        m_selected_square = result;
        this->update();
        emit bidSelected();
    }
}

Bid BidGrid::selectedBid() {
    uint i = m_selected_square % 6;
    uint j = m_selected_square / 6;
    switch(i) {
    case 0:
        return Bid(Suit::SPADES, j+6);
    case 1:
        return Bid(Suit::CLUBS, j+6);
    case 2:
        return Bid(Suit::DIAMONDS, j+6);
    case 3:
        return Bid(Suit::HEARTS, j+6);
    case 4:
        return Bid(Suit::NONE, j+6);
    case 5:
        if(j == 1) return Bid(Bid::CLOSED_MISERE);
        else if(j==3) return Bid(Bid::OPEN_MISERE);
    }
    fatal(error<<"Invalid bid selection");
}
