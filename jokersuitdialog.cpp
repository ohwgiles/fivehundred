#include "jokersuitdialog.hpp"
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
