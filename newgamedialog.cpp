#include "newgamedialog.hpp"
#include "ui_newgamedialog.h"

#include "setupplayer.hpp"

#include "player.hpp"

NewGameDialog::NewGameDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewGameDialog)
{
    ui->setupUi(this);
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
