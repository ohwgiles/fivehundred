/*!
    \file main.cpp
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
#include <QtGui/QApplication>
#include <QStringList>
#include "mainwindow.hpp"
#include "scorechart.hpp"
#include "deck.hpp"
#include "game.hpp"
#include "computer.hpp"
#include "log.hpp"
#include "os.hpp"

void usage() {
    error << "Usage TODO";
}

int main(int argc, char *argv[])
{
    trace;

    QApplication a(argc, argv);
    qRegisterMetaType<Seat>("Seat");
    qRegisterMetaType<std::vector<Card*>>("std::vector<Card*>");

    bool interactive = true;
    bool open_hand = false;
    QString names[4] = {"South", "West", "North", "East"};
    QString scripts[4];
    for(QString& s: scripts) {
        s = QString(os::AI_PATH) + "Jed.lua";
    };

    QStringList arglist = a.arguments();
    for(int i=1; i<arglist.length(); ++i) {
        if(arglist[i] == "--noninteractive" || arglist[i] == "-n")
            interactive = false;
        else if(arglist[i] == "--loglevel" || arglist[i] == "-l") {
            if(Log::setLogLevel(arglist[++i]) != true)
                fatal(error<<"Unrecognised loglevel: " << arglist[i]);
        }
        else if(arglist[i] == "--open-hand" || arglist[i] == "-o")
            open_hand = true;
        else if(arglist[i] == "--south-name")
            names[SOUTH] = arglist[++i];
        else if(arglist[i] == "--west-name")
            names[WEST] = arglist[++i];
        else if(arglist[i] == "--north-name")
            names[NORTH] = arglist[++i];
        else if(arglist[i] == "--east-name")
            names[EAST] = arglist[++i];
        else if(arglist[i] == "--south-ai")
            scripts[SOUTH] = arglist[++i];
        else if(arglist[i] == "--west-ai")
            scripts[WEST] = arglist[++i];
        else if(arglist[i] == "--north-ai")
            scripts[NORTH] = arglist[++i];
        else if(arglist[i] == "--east-ai")
            scripts[EAST] = arglist[++i];

        else {
            usage();
            return 1;
        }
    }

    if(interactive) {
        a.setWindowIcon(QIcon(QString(os::GFX_PATH) + "icon24.png"));
        info << "Running in interactive mode";
        MainWindow gui(open_hand);
        return a.exec();
    } else {
        info << "Running in non-interactive mode";
        Deck d;
        Game g(&d);
        g.addPlayer(new Computer(SOUTH, names[SOUTH], scripts[SOUTH], open_hand));
        g.addPlayer(new Computer(WEST, names[WEST], scripts[WEST], open_hand));
        g.addPlayer(new Computer(NORTH, names[NORTH], scripts[NORTH], open_hand));
        g.addPlayer(new Computer(EAST, names[EAST], scripts[EAST], open_hand));
        g.run();

        info << "Exiting...";
    }
}
