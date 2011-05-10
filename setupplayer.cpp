#include "setupplayer.hpp"
#include "ui_setupplayer.h"

#include "os.hpp"

#include <QDir>
#include "log.hpp"
SetupPlayer::SetupPlayer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SetupPlayer)
{
    trace;
    ui->setupUi(this);
    ui->script->hide();
}

SetupPlayer::~SetupPlayer()
{
    trace;
    delete ui;
}

void SetupPlayer::setPlayerType(Type type) {
    trace;
    ui->playerType->setCurrentIndex(int(type));
}

SetupPlayer::Type SetupPlayer::getPlayerType() const {
    trace;
    return ui->playerType->currentText() == "Human" ? HUMAN : COMPUTER;
}
QString SetupPlayer::getScript() const {
    trace;
    return QString(os::AI_PATH) + "/" + ui->script->currentText() + ".lua";
}
QString SetupPlayer::getName() const {
    trace;
    return ui->playerName->text();
}

void SetupPlayer::on_playerType_currentIndexChanged(QString type)
{
    trace;
    if(type == "Human") {
        ui->script->hide();
        ui->playerName->setText(os::getUserName());
    } else if(type == "Computer") {
        ui->script->clear();
        QDir dir(os::AI_PATH);
        QStringList scriptfiles;
        scriptfiles << "*.lua";
        dir.setNameFilters(scriptfiles);
        QStringList scripts = dir.entryList();
        for(QString s: scripts) {
            s.remove(".lua");
            ui->script->addItem(s);
        }
        ui->script->show();
    }
}

void SetupPlayer::on_script_currentIndexChanged(QString s)
{
    trace;
    ui->playerName->setText(s);
}
