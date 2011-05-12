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
#include "player.hpp"
#include "bidding.hpp"
#include <QVariant>
#include <sstream>
#include "log.hpp"

BidDialog::BidDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BidDialog)
{
    trace;
    ui->setupUi(this);

    ui->tableWidget->setColumnCount(2);
    ui->tableWidget->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    ui->tableWidget->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);

    connect(this, SIGNAL(accepted()), this, SLOT(dialog_accepted()));
    connect(this, SIGNAL(rejected()), this, SLOT(dialog_rejected()));
}

BidDialog::~BidDialog()
{
    trace;
    delete ui;
}

void BidDialog::show(Human* player, Bidding* bids) {
    debug;
    m_player = player;

    Bid winner(bids->maxBid());

    ui->r6s->setEnabled(winner < Bid(Suit::SPADES, 6));
    ui->r7s->setEnabled(winner < Bid(Suit::SPADES, 7));
    ui->r8s->setEnabled(winner < Bid(Suit::SPADES, 8));
    ui->r9s->setEnabled(winner < Bid(Suit::SPADES, 9));
    ui->r10s->setEnabled(winner < Bid(Suit::SPADES, 10));

    ui->r6c->setEnabled(winner < Bid(Suit::CLUBS, 6));
    ui->r7c->setEnabled(winner < Bid(Suit::CLUBS, 7));
    ui->r8c->setEnabled(winner < Bid(Suit::CLUBS, 8));
    ui->r9c->setEnabled(winner < Bid(Suit::CLUBS, 9));
    ui->r10c->setEnabled(winner < Bid(Suit::CLUBS, 10));

    ui->r6d->setEnabled(winner < Bid(Suit::DIAMONDS, 6));
    ui->r7d->setEnabled(winner < Bid(Suit::DIAMONDS, 7));
    ui->r8d->setEnabled(winner < Bid(Suit::DIAMONDS, 8));
    ui->r9d->setEnabled(winner < Bid(Suit::DIAMONDS, 9));
    ui->r10d->setEnabled(winner < Bid(Suit::DIAMONDS, 10));

    ui->r6h->setEnabled(winner < Bid(Suit::HEARTS, 6));
    ui->r7h->setEnabled(winner < Bid(Suit::HEARTS, 7));
    ui->r8h->setEnabled(winner < Bid(Suit::HEARTS, 8));
    ui->r9h->setEnabled(winner < Bid(Suit::HEARTS, 9));
    ui->r10h->setEnabled(winner < Bid(Suit::HEARTS, 10));

    ui->r6n->setEnabled(winner < Bid(Suit::NONE, 6));
    ui->r7n->setEnabled(winner < Bid(Suit::NONE, 7));
    ui->r8n->setEnabled(winner < Bid(Suit::NONE, 8));
    ui->r9n->setEnabled(winner < Bid(Suit::NONE, 9));
    ui->r10n->setEnabled(winner < Bid(Suit::NONE, 10));

    ui->rcm->setEnabled(winner < Bid(Bid::CLOSED_MISERE));
    ui->rom->setEnabled(winner < Bid(Bid::OPEN_MISERE));

    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(bids->count());
    for(unsigned i=0; i<bids->count(); ++i) {
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(bids->at(i).player->name));
        std::stringstream ss;
        ss << bids->at(i).bid;
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(ss.str().c_str()));
    }
    ui->tableWidget->reset();

    QDialog::show();
}

void BidDialog::on_pushButton_clicked()
{
    trace;
        this->accept();
}

void BidDialog::on_pushButton_2_clicked()
{
    trace;
    this->reject();
}

void BidDialog::dialog_accepted() {
    Bid bid(Bid::PASS);

    if(ui->r6s->isChecked()) bid = Bid(Suit::SPADES, 6);
    if(ui->r7s->isChecked()) bid = Bid(Suit::SPADES, 7);
    if(ui->r8s->isChecked()) bid = Bid(Suit::SPADES, 8);
    if(ui->r9s->isChecked()) bid = Bid(Suit::SPADES, 9);
    if(ui->r10s->isChecked()) bid = Bid(Suit::SPADES, 10);

    if(ui->r6c->isChecked()) bid = Bid(Suit::CLUBS, 6);
    if(ui->r7c->isChecked()) bid = Bid(Suit::CLUBS, 7);
    if(ui->r8c->isChecked()) bid = Bid(Suit::CLUBS, 8);
    if(ui->r9c->isChecked()) bid = Bid(Suit::CLUBS, 9);
    if(ui->r10c->isChecked()) bid = Bid(Suit::CLUBS, 10);

    if(ui->r6d->isChecked()) bid = Bid(Suit::DIAMONDS, 6);
    if(ui->r7d->isChecked()) bid = Bid(Suit::DIAMONDS, 7);
    if(ui->r8d->isChecked()) bid = Bid(Suit::DIAMONDS, 8);
    if(ui->r9d->isChecked()) bid = Bid(Suit::DIAMONDS, 9);
    if(ui->r10d->isChecked()) bid = Bid(Suit::DIAMONDS, 10);

    if(ui->r6h->isChecked()) bid = Bid(Suit::HEARTS, 6);
    if(ui->r7h->isChecked()) bid = Bid(Suit::HEARTS, 7);
    if(ui->r8h->isChecked()) bid = Bid(Suit::HEARTS, 8);
    if(ui->r9h->isChecked()) bid = Bid(Suit::HEARTS, 9);
    if(ui->r10h->isChecked()) bid = Bid(Suit::HEARTS, 10);

    if(ui->r6n->isChecked()) bid = Bid(Suit::NONE, 6);
    if(ui->r7n->isChecked()) bid = Bid(Suit::NONE, 7);
    if(ui->r8n->isChecked()) bid = Bid(Suit::NONE, 8);
    if(ui->r9n->isChecked()) bid = Bid(Suit::NONE, 9);
    if(ui->r10n->isChecked()) bid = Bid(Suit::NONE, 10);

    if(ui->rcm->isChecked()) bid = Bid(Bid::CLOSED_MISERE);
    if(ui->rom->isChecked()) bid = Bid(Bid::OPEN_MISERE);

    emit(bidMade(m_player, bid));
}

void BidDialog::dialog_rejected()
{
    emit(bidMade(m_player, Bid(Bid::PASS)));
    debug;
}
