#ifndef SETUPPLAYER_HPP
#define SETUPPLAYER_HPP
/*!
  \file setupplayer.hpp
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
#include <QWidget>

namespace Ui {
    class SetupPlayer;
}

/*!
  \class SetupPlayer
  \brief Widget for creating a new player

  This widget provides a combo box for selecting whether a player is
  Human or Computer, a line edit for setting the player name, and if
  the player is a computer, a combo box for setting the AI script
*/
class SetupPlayer : public QWidget
{
    Q_OBJECT

public:
    explicit SetupPlayer(QWidget *parent = 0);
    ~SetupPlayer();

    enum Type { HUMAN = 0, COMPUTER };

    //! Causes the UI to display default controls and values for the given Player type
    void setPlayerType(Type type);

    Type getPlayerType() const;
    QString getScript() const;
    QString getName() const;

private slots:
    void on_playerType_currentIndexChanged(QString);
    void on_script_currentIndexChanged(QString);

private:
    Ui::SetupPlayer *ui;
};

#endif // SETUPPLAYER_HPP
