/*!
  \file bidgrid.hpp
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
#ifndef BIDGRID_HPP
#define BIDGRID_HPP

#include <QWidget>

class Bid;

class BidGrid : public QWidget
{
    Q_OBJECT
public:
    explicit BidGrid(QWidget *parent = 0);
    void setWinningBid(const Bid& bid);
    Bid selectedBid();

protected:
    void paintEvent(QPaintEvent *);
    void drawRect(QPainter& painter, uint i, uint j);
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    int mousePosToIndex(QMouseEvent* ev);
    enum { POS_UNSET = -2, POS_NONE = -1 };
    int coordToIndex(uint i, uint j) { return 6*j + i; }
    int m_mouse_square;
    int m_selected_square;
    int m_disabled_threshold;
    QPixmap m_grid_image;
    static const uint X_OFFSET = 61;
    static const uint Y_OFFSET = 48;
    static const uint X_SIZE = 60;
    static const uint Y_SIZE = 40;

signals:
    void bidSelected();

public slots:

};

#endif // BIDGRID_HPP
