#ifndef JOKERSUITDIALOG_HPP
#define JOKERSUITDIALOG_HPP
/*!
  \file jokersuitdialog.hpp
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
#include "suit.hpp"

class Player;

namespace Ui {
    class JokerSuitDialog;
}

/*!
  \class JokerSuitDialog
  \brief Dialog box to select the suit of the joker

  When leading the joker in No Trumps, the suit must be designated.
  This class handles displaying the dialog and disabling selection
  of the suits in which the player has already indicated short
*/
class JokerSuitDialog : public QDialog
{
    Q_OBJECT

public:
    explicit JokerSuitDialog(Player * player, QWidget *parent = 0);
    ~JokerSuitDialog();

    //! Get the result
    Suit chosenSuit() { return m_desired_suit; }

private slots:
    void on_buttonCancel_clicked();
    void on_buttonSpades_clicked();
    void on_buttonClubs_clicked();
    void on_buttonDiamonds_clicked();
    void on_buttonHearts_clicked();

private:
    Ui::JokerSuitDialog *ui;
    Suit m_desired_suit;
};

#endif // JOKERSUITDIALOG_HPP
