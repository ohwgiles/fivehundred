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
#include <QDialog>

#include "bid.hpp"

class Bidding;
class Human;

namespace Ui {
    class BidDialog;
}

class BidDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BidDialog(QWidget *parent = 0);
    ~BidDialog();

    void show(Human* player, Bidding* bids);

private:
    Ui::BidDialog *ui;
    Human* m_player;

signals:
    void bidMade(Human* player, Bid);

private slots:
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();
    void dialog_accepted();
    void dialog_rejected();
};

#endif // BIDDIALOG_HPP
