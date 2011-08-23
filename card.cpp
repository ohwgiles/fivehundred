#include "card.hpp"
/*!
  \file card.cpp
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
#include "os.hpp"
#include <QGraphicsSceneMouseEvent>
#include <QSize>
#include <iostream>
#include "log.hpp"

QPixmap* Card::back_pixmap(0);

void Card::createBackPixmap() {
    back_pixmap = new QPixmap(QString(os::GFX_PATH) + "back.gif");
}

Card::Card(Suit suit, Value val):
    QObject(),
    SimpleCard(suit, val),
    QGraphicsPixmapItem(),
    m_pixmap(findImage(m_suit,m_value)),
    m_face_up(false),
    m_raised(false)
{
    trace;
    if(back_pixmap)
        setPixmap(*back_pixmap);
    setOffset(-1*pixmap().width()/2, -1*pixmap().height()/2);
    update();
}

Card::Card(const Card& other) :
    QObject(),
    SimpleCard(other.m_suit, other.m_value),
    QGraphicsPixmapItem(),
    m_pixmap(other.m_pixmap),
    m_face_up(false),
    m_raised(false)
{
    trace;
    if(back_pixmap)
        setPixmap(*back_pixmap);
    setOffset(-1*pixmap().width()/2, -1*pixmap().height()/2);
    update();
}

Card::~Card() {
}

QString Card::findImage(Suit suit, int value) {
    if(value==JOKER) return QString(os::GFX_PATH) + "53.gif";
    int cardnum = (suit==Suit::CLUBS ? 0 : suit == Suit::DIAMONDS ? 1 : suit == Suit::HEARTS ? 2 : 3)*13 + value - 1;
    return QString(os::GFX_PATH) + (cardnum<=9?"0":"")+QString::number(cardnum)+".gif";
}


void Card::setFaceUp(bool setFaceUp) {
    trace;
    //m_new_display.faceup = setFaceUp;
    if(m_face_up != setFaceUp) {
        if(setFaceUp) {
            setPixmap(m_pixmap);
        } else {
            setPixmap(*back_pixmap);
        }
        m_face_up = setFaceUp;
    }
    update();
}

void Card::raise(bool v) {
    trace;
    m_raised = v;
}

bool Card::raised() const {
    trace;
    return m_raised;
}


void Card::mousePressEvent(QGraphicsSceneMouseEvent*) {
    trace;
    emit cardClicked(*this);
}
