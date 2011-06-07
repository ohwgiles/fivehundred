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

const char Card::className[] = "Card";

QPixmap* Card::back_pixmap(0);

void Card::createBackPixmap() {
    back_pixmap = new QPixmap(QString(os::GFX_PATH) + "back.gif");
}

Card::Card() :
    m_suit(Suit::NONE),
    m_value(Card::Value(0)),
    m_face_up(false),
    m_raised(false)
{
    fatal(error << "cannot use default Card constructor");
}

Card::Card(Suit suit, Value val):
    QObject(),
    QGraphicsPixmapItem(),
    m_suit(suit),
    m_value(val),
    m_pixmap(findImage(suit,val)),
    m_face_up(false),
    m_raised(false)
{
    trace;
    if(suit == Suit::NONE && val != JOKER)
        fatal(error<<"Invalid parameters to Card ctor");
    setPixmap(*back_pixmap);
    setOffset(-1*pixmap().width()/2, -1*pixmap().height()/2);
    update();
}

Card::~Card() {}

QString Card::findImage(Suit suit, int value) {
    if(value==JOKER) return QString(os::GFX_PATH) + "53.gif";
    int cardnum = (suit==Suit::CLUBS ? 0 : suit == Suit::DIAMONDS ? 1 : suit == Suit::HEARTS ? 2 : 3)*13 + value - 1;
    return QString(os::GFX_PATH) + (cardnum<=9?"0":"")+QString::number(cardnum)+".gif";
}


Card& Card::operator=(const Card& copy) {
    trace;
    m_suit = copy.m_suit;
    m_value = copy.m_value;
    return *this;
}

bool Card::operator==(const Card& other) const {
    trace;
    return m_suit == other.m_suit;
    return m_value == other.m_value;
}

bool Card::isTrump(Suit trumps) const {
    trace;
    if(m_value == JOKER) return true;
    if(m_suit == trumps) return true;
    if(m_value == JACK && m_suit == trumps.complementary()) return true;
    return false;
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

Suit Card::suit(Suit trump) const {
    if(m_value == JOKER) return trump;
    else if(trump != Suit::NONE && m_value == JACK && m_suit == trump.complementary()) return trump;
    else return m_suit;
}
Card::Value Card::value(Suit trump) const {
    if (m_value == JACK && suit(trump) == trump) return m_suit == trump.complementary() ? LEFT_BOWER : RIGHT_BOWER;
    else return m_value;
}



std::ostream& operator<<(std::ostream& s, const Card& c) {
    trace;
    s << "the ";
    if(c.m_value == Card::JOKER) {
        s << "Joker";
        return s;
    } else if(c.m_value < Card::JACK)
        s << c.m_value << " of ";
    else if(c.m_value == Card::JACK)
        s << "Jack of ";
    else if(c.m_value == Card::QUEEN)
        s << "Queen of ";
    else if(c.m_value == Card::KING)
        s << "King of ";
    else if(c.m_value == Card::ACE)
        s << "Ace of ";
    s << c.m_suit;
    return s;
}

std::istream& operator>>(std::istream& is, Card& c) {
    trace;
    std::string tmp;
    is >> tmp;
    if(tmp != "the") is.setstate(is.badbit);

    is >> tmp;
    Card::Value v;
    if(tmp == "Joker") {
        c = Card(Suit::NONE, Card::JOKER);
        return is;
    } else if(tmp == "Jack") {
        v = Card::JACK;
    } else if(tmp == "Queen") {
        v = Card::QUEEN;
    } else if(tmp == "King") {
        v = Card::KING;
    } else if(tmp == "Ace") {
        v = Card::ACE;
    } else {
        std::stringstream ss(tmp);
        unsigned tmp_value;
        ss >> tmp_value;
        if(ss.fail() || !(tmp_value >= Card::TWO && tmp_value <= Card::TEN)) {
            is.setstate(is.failbit);
            return is;
        } else {
            v = Card::Value(tmp_value);
        }
    }

    is >> tmp;
    if(tmp != "of") is.setstate(is.badbit);
    Suit e;
    if (!(is >> e).fail()) {
        c = Card(e, v);
    }
    return is;
}

void Card::mousePressEvent(QGraphicsSceneMouseEvent*) {
    trace;
    emit cardClicked(*this);
}
