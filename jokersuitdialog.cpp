#include "jokersuitdialog.hpp"
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
#include "ui_jokersuitdialog.h"
#include "player.hpp"
#include "log.hpp"

JokerSuitDialog::JokerSuitDialog(Player* player, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::JokerSuitDialog)
{
    trace;
    ui->setupUi(this);
    ui->buttonSpades->setEnabled(!player->offsuitPlayed.has(Suit::SPADES));
    ui->buttonClubs->setEnabled(!player->offsuitPlayed.has(Suit::CLUBS));
    ui->buttonDiamonds->setEnabled(!player->offsuitPlayed.has(Suit::DIAMONDS));
    ui->buttonHearts->setEnabled(!player->offsuitPlayed.has(Suit::HEARTS));
}

JokerSuitDialog::~JokerSuitDialog() {
    trace;
    delete ui;
}

void JokerSuitDialog::on_buttonCancel_clicked() {
    trace;
    reject();
}

void JokerSuitDialog::on_buttonSpades_clicked() {
    trace;
    m_desired_suit = Suit::SPADES;
    accept();
}

void JokerSuitDialog::on_buttonClubs_clicked()
{
    trace;
    m_desired_suit = Suit::CLUBS;
    accept();
}

void JokerSuitDialog::on_buttonDiamonds_clicked()
{
    trace;
    m_desired_suit = Suit::DIAMONDS;
    accept();
}

void JokerSuitDialog::on_buttonHearts_clicked()
{
    trace;
    m_desired_suit = Suit::HEARTS;
    accept();
}
