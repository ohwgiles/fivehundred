#ifndef NEWGAMEDIALOG_HPP
#define NEWGAMEDIALOG_HPP
/*!
  \file newgamedialog.hpp
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

namespace Ui {
    class NewGameDialog;
}

class SetupPlayer;

/*!
  \class NewGameDialog
  \brief The dialog that pops up when you start a new game

  This dialog allows you to select the four players as Human or Computer,
  their names, and if Computer, their script files. It defaults to position
  south being Human and the rest Computer.

  \see SetupPlayer
*/
class NewGameDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewGameDialog(QWidget *parent = 0);
    ~NewGameDialog();

    //! Untidy function that returns an array of pointers to SetupPlayers
    //! Its intent is to allow the allow the MainWindow to instantiate the appropriate Player s
    SetupPlayer** result();

private:
    Ui::NewGameDialog *ui;
};

#endif // NEWGAMEDIALOG_HPP
