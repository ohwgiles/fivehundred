#ifndef BUBBLE_HPP
#define BUBBLE_HPP
/*!
  \file bubble.hpp
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

#include <QGraphicsItem>
#include <QObject>
#include <QFont>
#include "seat.hpp"

class Bubble : public QObject, public QGraphicsItem {
    Q_OBJECT
    Q_PROPERTY(qreal scale READ scale WRITE setScale)
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)
public:
    Bubble(Seat orientation);
    void setText(const QString& str);
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    static const int X_MID = 67;
    static const int Y_MID = 26;

protected:

    QPoint m_offset;
    QPixmap m_bubble;
    QString m_text;
    unsigned m_text_yo;
    QFont m_font;
};

#endif // BUBBLE_HPP
