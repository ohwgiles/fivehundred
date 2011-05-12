#include "setupplayer.hpp"
/*!
  \file setupplayer.cpp
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
        // Add all the .lua files in the AI path to the combo box
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
