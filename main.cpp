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
#include <iostream>

struct Arg {
    Arg() :
        numArgs(0),
        supplied(false)
    {}
    Arg(QString shortName, QString desc, int args=0) :
        shortName(shortName), description(desc), numArgs(args), supplied(false)
    {}
    QString shortName;
    QString description;
    int numArgs;
    bool supplied;
    QStringList args;
};

void usage(std::map<QString, Arg>& args, int exitcode) {
    std::cout << "Five Hundred card game\nUsage:\n";
    for(std::pair<const QString, Arg>& arg : args) {

        std::cout << "  " << arg.first;
        if(arg.second.shortName != "") std::cout << "," << arg.second.shortName;
        for(int i=0; i<arg.second.numArgs; ++i) std::cout << " <string>";
        std::cout << "\n      " << arg.second.description << "\n";
    }
    exit(exitcode);
}

int main(int argc, char *argv[])
{
    trace;

    QApplication a(argc, argv);
    qRegisterMetaType<Seat>("Seat");
    qRegisterMetaType<Bid>("Bid");
    qRegisterMetaType<std::vector<Card*>>("std::vector<Card*>");

    std::map<QString, Arg> args;
    args["--help"] = Arg("-h","Display usage message");
    args["--non-interactive"] = Arg("-n","Run without a GUI");
    args["--open-hand"] = Arg("-o","Computers play open hand");
    args["--loglevel"] = Arg("-l","logging level (TRACE, DEBUG, USER, INFO, ERROR)",1);
    args["--south-name"] = Arg("","Name of player at South",1);
    args["--east-name"] = Arg("","Name of player at East",1);
    args["--north-name"] = Arg("","Name of player at North",1);
    args["--west-name"] = Arg("","Name of player at West",1);
    args["--south-ai"] = Arg("", "Path to Lua script to use for South AI",1);
    args["--east-ai"] = Arg("", "Path to Lua script to use for East AI",1);
    args["--north-ai"] = Arg("", "Path to Lua script to use for North AI",1);
    args["--west-ai"] = Arg("", "Path to Lua script to use for West AI",1);

    QStringList arglist = a.arguments();
    for(int i=1; i<arglist.length(); ++i) {

        for(std::pair<const QString, Arg>& arg : args) {
            if(arglist[i] == arg.first || (arg.second.shortName != "" && arglist[i] == arg.second.shortName)) {
                arg.second.supplied = true;
                if(arg.second.numArgs > arglist.length() - i - 1) {
                    error<<"Not enough arguments supplied to " << arglist[i];
                    usage(args, 1);
                }
                for(int j=0; j<arg.second.numArgs; ++j)
                    arg.second.args << arglist[++i];
                goto next;
            }
        }
        error<<"Unrecognised argument: " << arglist[i];
        usage(args,1);
        next:
            continue;
    }

    if(args["--help"].supplied) {
        usage(args, 0);
    }

    if(args["--loglevel"].supplied) {
        QString level = args["--loglevel"].args.first();
        if(Log::setLogLevel(level) != true) {
            error<<"Unrecognised loglevel: " << level;
            usage(args, 1);
        }
    }

    for(QString str : {"--south-name","--west-name","--north-name","--east-name"}) {
        if(!args[str].supplied) {
            QString s = str;
            s.remove("-name");
            s.remove("--");
            args[str].args << s;
        }
    }

    for(QString str : {"--south-ai","--west-ai","--north-ai","--east-ai"}) {
        if(!args[str].supplied)
            args[str].args << QString(os::AI_PATH) + "Patrick.lua";
    }

    bool open_hand = args["--open-hand"].supplied;
    if(!args["--non-interactive"].supplied) {
        a.setWindowIcon(QIcon(QString(os::GFX_PATH) + "icon.gif"));
        info << "Running in interactive mode";
        MainWindow gui(open_hand);
        return a.exec();
    } else {
        info << "Running in non-interactive mode";
        Deck d;
        Game g(&d);
        g.addPlayer(new Computer(SOUTH, args["--south-name"].args.first(),  args["--south-ai"].args.first(), open_hand));
        g.addPlayer(new Computer(WEST, args["--west-name"].args.first(), args["--west-ai"].args.first(), open_hand));
        g.addPlayer(new Computer(NORTH, args["--north-name"].args.first(), args["--north-ai"].args.first(), open_hand));
        g.addPlayer(new Computer(EAST, args["--east-name"].args.first(), args["--east-ai"].args.first(), open_hand));
        g.run();

        info << "Exiting...";
    }
}
