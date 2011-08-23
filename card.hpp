#ifndef CARD_H
#define CARD_H
/*!
  \file card.hpp
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
#include "simplecard.hpp"
#include "seat.hpp"
#include <QObject>
#include <QGraphicsPixmapItem>
#include <iosfwd>

// Forward Declarations
class QGraphicsSceneMouseEvent;

/*!
  \class Card
  \brief Playing card object

  Complex object that knows too much about what's going on outside its world
*/
class Card : public QObject, public SimpleCard, public QGraphicsPixmapItem {
    Q_OBJECT
    Q_PROPERTY(QPointF position READ pos WRITE setPos)
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)
public:
    enum Location { HAND, TRICK, HIDDEN };

    static const unsigned H_OFFSET = 20;
    static const unsigned HEIGHT = 116;
    static const unsigned WIDTH = 80;
    static const unsigned EDGE_OFFSET = 100;
    static const unsigned RAISE = 20;
    static const unsigned WON_OFFSET = 160;

    Card(Suit suit = Suit::NONE, Value val = INVALID);
    Card(const Card& other);
    Card& operator=(const Card& copy) { return (Card&)SimpleCard::operator=(copy); }

    bool operator==(const Card& other) const { return SimpleCard::operator==(other); }
    bool operator==(const Card* other) const { return SimpleCard::operator==(other); }
    bool operator==(const SimpleCard& other) const { return SimpleCard::operator==(other); }
    bool operator==(const SimpleCard* other) const { return SimpleCard::operator==(other); }

    virtual ~Card();

    void setFaceUp(bool setFaceUp=true);
    void raise(bool v=true);
    bool raised() const;

    static void createBackPixmap();

signals:
    void cardClicked(Card& card);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event);

    QPixmap m_pixmap;
    static QPixmap* back_pixmap;
    bool m_face_up;
    bool m_raised;

    static QString findImage(Suit suit, int value);

};


#endif // CARD_H
