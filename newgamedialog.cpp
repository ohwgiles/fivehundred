#include "newgamedialog.hpp"
/*!
  \file newgamedialog.cpp
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
#include "ui_newgamedialog.h"
#include "setupplayer.hpp"
#include "player.hpp"

NewGameDialog::NewGameDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewGameDialog)
{
    ui->setupUi(this);
    // Set up default values
    ui->north->setPlayerType(SetupPlayer::COMPUTER);
    ui->east->setPlayerType(SetupPlayer::COMPUTER);
    ui->west->setPlayerType(SetupPlayer::COMPUTER);
    ui->south->setPlayerType(SetupPlayer::HUMAN);
}

SetupPlayer** NewGameDialog::result() {
    SetupPlayer** setup = new SetupPlayer*[4];
    setup[NORTH] = ui->north;
    setup[EAST] = ui->east;
    setup[WEST] = ui->west;
    setup[SOUTH] = ui->south;
    return setup;
}

NewGameDialog::~NewGameDialog()
{
    delete ui;
}
