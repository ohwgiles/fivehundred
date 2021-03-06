#ifndef BIDDIALOG_HPP
#define BIDDIALOG_HPP
/*!
  \file biddialog.hpp
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
#include "bid.hpp"
#include <QDialog>
#include <QPoint>

//Forward Declarations
class Bidding;
class Human;

namespace Ui {
    class BidDialog;
}

/*!
  \class BidDialog
  \brief The dialog that asks which bid you want to place
*/
class BidDialog : public QDialog {
    Q_OBJECT
public:
    explicit BidDialog(QWidget *parent = 0);
    ~BidDialog();

protected slots:
    void show(Human* player, Bidding* bids);

protected:
    virtual void moveEvent(QMoveEvent *);
    QPoint m_pos;
    Ui::BidDialog *ui;
    Human* m_player;

signals:
    void bidMade(Human* player, Bid);

private slots:
    void bidSelected();
    void on_buttonPass_clicked();
    void on_buttonBid_clicked();
    void dialog_rejected();
    void on_buttonClosedMisere_clicked();
    void on_buttonOpenMisere_clicked();
};

#endif // BIDDIALOG_HPP
