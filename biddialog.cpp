#include "biddialog.hpp"
/*!
  \file biddialog.cpp
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
#include "ui_biddialog.h"
#include "human.hpp"
#include "bidding.hpp"
#include <QVariant>
#include <QMoveEvent>
#include <QMessageBox>
#include <sstream>
#include "bidgrid.hpp"
#include "log.hpp"

BidDialog::BidDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BidDialog)
{
    trace;
    ui->setupUi(this);

    this->resize(ui->bidgrid->size());

    connect(this, SIGNAL(rejected()), this, SLOT(dialog_rejected()));
    connect(ui->bidgrid, SIGNAL(bidSelected()), this, SLOT(bidSelected()));
}

BidDialog::~BidDialog()
{
    trace;
    delete ui;
}

void BidDialog::show(Human* player, Bidding* bids) {
    debug;
    m_player = player;
    ui->buttonBid->setEnabled(false);

    if(bids->hasWinner() && bids->winner().player == static_cast<Player*>(player)) {
        if(QMessageBox::question(0, "You have won the bid",
            "Would you like to raise your bid? If you change suit, other players may bid again.",
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::No) {
            this->reject();
            return;
        }
    }

    Bid winningBid = bids->maxBid();
    ui->buttonClosedMisere->setEnabled(Bid(Bid::CLOSED_MISERE).worth() > winningBid.worth());
    ui->buttonOpenMisere->setEnabled(Bid(Bid::OPEN_MISERE).worth() > winningBid.worth());
    ui->bidgrid->setWinningBid(winningBid);

    this->move(m_pos);
    QDialog::show();
}

void BidDialog::on_buttonBid_clicked()
{
    trace;
    Bid bid = ui->bidgrid->selectedBid();
    emit(bidMade(m_player, bid));
    accept();
}

void BidDialog::on_buttonPass_clicked()
{
    trace;
    reject();
}

void BidDialog::moveEvent(QMoveEvent* event) {
    QDialog::moveEvent(event);
    m_pos = event->pos();
}

void BidDialog::bidSelected() {
    ui->buttonBid->setEnabled(true);
}

void BidDialog::dialog_rejected() {
    emit(bidMade(m_player, Bid(Bid::PASS)));
    debug;
}

void BidDialog::on_buttonClosedMisere_clicked() {
    emit bidMade(m_player, Bid(Bid::CLOSED_MISERE));
    accept();
}

void BidDialog::on_buttonOpenMisere_clicked() {
    emit bidMade(m_player, Bid(Bid::OPEN_MISERE));
    accept();
}
