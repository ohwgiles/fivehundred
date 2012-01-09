#include "mainwindow.hpp"
/*!
  \file mainwindow.cpp
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
#include "ui_mainwindow.h"

#include <QResizeEvent>
#include <QMessageBox>
#include <QPushButton>
#include <QPropertyAnimation>
#include <QGraphicsProxyWidget>
#include <QParallelAnimationGroup>
#include <QDesktopServices>
#include <QUrl>
#include <vector>

#include "setupplayer.hpp"
#include "newgamedialog.hpp"
#include "scorechart.hpp"

#include "deck.hpp"
#include "contract.hpp"
#include "game.hpp"
#include "biddialog.hpp"
#include "human.hpp"
#include "computer.hpp"
#include "os.hpp"
#include "version.h"
#include "log.hpp"

MainWindow::MainWindow(bool open_hand, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_kittybutton(new QPushButton("Discard Kitty", 0)),
    m_biddialog(new BidDialog(this)),
    m_game(0),
    m_table(m_kittybutton),
    m_open_hand(open_hand)
{
    ui->setupUi(this);

    ui->graphicsView->setScene(&m_table);

    // It would be nice to use direct rendering some time maybe?
    // QGLWidget* gl = new QGLWidget(QGLFormat(QGL::DirectRendering));
    //ui->graphicsView->setViewport(gl);
    ui->graphicsView->setStyleSheet(QString("background-image: url('") + os::GFX_PATH + "table.gif');");

    // Add all extra widgets to scene
    ui->statusBar->addWidget(&m_lbl_status, 1);
    ui->statusBar->addWidget(&m_lbl_team_ns);
    ui->statusBar->addWidget(&m_lbl_tricks_ns);
    ui->statusBar->addWidget(&m_lbl_score_ns);
    ui->statusBar->addWidget(&m_lbl_team_ew);
    ui->statusBar->addWidget(&m_lbl_tricks_ew);
    ui->statusBar->addWidget(&m_lbl_score_ew);
    m_lbl_bid.setAlignment(Qt::AlignRight);
    ui->statusBar->addWidget(&m_lbl_bid, 1);

    resetUI();
    show();

    // Still not quite sure why we have to do this
    resize(m_table.sceneRect().size().toSize());
}

MainWindow::~MainWindow() {
    trace;
    delete m_biddialog;
    delete m_kittybutton;
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);
    QSize size(ui->graphicsView->size());
    m_table.reposition(size);
    //m_deck->reposition(size);
    if(m_game) {
        m_game->reposition();
    }
}



void MainWindow::on_actionNew_Game_triggered()
{
    trace;
    if(m_game) {
        on_actionEnd_Game_triggered();
        m_next_action = NEW_GAME;
    } else {
        m_next_action = NONE;
        newGame();
    }
}

void MainWindow::newGame() {
    trace;
    info << "Starting a new game";

    NewGameDialog d;
    if(d.exec() == QDialog::Accepted) {
        m_game = new Game(m_table.deck());

        SetupPlayer** cfg = d.result();
        for(unsigned i=0; i<4; ++i) {
            Player* p;
            if(cfg[Seat(i)]->getPlayerType() == SetupPlayer::HUMAN) {
                Human* h = new Human(Seat(i), cfg[Seat(i)]->getName());
                //connect(h, SIGNAL(updateScene()), this, SLOT(updateDisplay()), Qt::QueuedConnection);
                connect(h, SIGNAL(requestBidDialog(Human*,Bidding*)), m_biddialog, SLOT(show(Human*,Bidding*)));
                connect(m_biddialog, SIGNAL(bidMade(Human*,Bid)), h, SLOT(bidMade(Human*,Bid)));
                connect(h, SIGNAL(requestKittyButtonEnabled(bool)), m_kittybutton, SLOT(setEnabled(bool)));
                connect(h, SIGNAL(requestKittyButtonVisible(bool)), m_kittybutton, SLOT(setVisible(bool)));
                connect(m_kittybutton, SIGNAL(clicked()), h, SLOT(kittyButtonClicked()));
                p = h;
            } else {
                p = new Computer(Seat(i), cfg[Seat(i)]->getName(), cfg[Seat(i)]->getScript(), m_open_hand);
            }
            connect(p, SIGNAL(placeCard(Card*,Seat,uint,uint)), &m_table, SLOT(placeCardInHand(Card*,Seat,uint,uint)), Qt::QueuedConnection);
            connect(p, SIGNAL(turnUpCard(Card*,bool)), &m_table, SLOT(turnUpCard(Card*,bool)), Qt::QueuedConnection);
            m_game->addPlayer(p);
            m_table.setPlayerName(Seat(i), p->name());
        }

        //connect(m_game, SIGNAL(sceneUpdated()), this, SLOT(updateDisplay()));
        connect(m_game, SIGNAL(showCard(Card*,bool)), &m_table, SLOT(showCard(Card*,bool)));
        connect(m_game, SIGNAL(newContract(Contract*)), this, SLOT(connectContract(Contract*)), Qt::DirectConnection);
        connect(m_game, SIGNAL(finished()), this, SLOT(threadFinished()));
        connect(m_game, SIGNAL(updateNorthSouthScore(QString)), &m_lbl_score_ns, SLOT(setText(QString)));
        connect(m_game, SIGNAL(updateEastWestScore(QString)), &m_lbl_score_ew, SLOT(setText(QString)));
        connect(m_game, SIGNAL(contractComplete()), this, SLOT(showScores()));
        connect(m_game, SIGNAL(contractInvalidated()), this, SLOT(contractInvalid()));

        m_lbl_team_ns.setText(cfg[0]->getName() + "/" + cfg[2]->getName());
        m_lbl_team_ew.setText(cfg[1]->getName() + "/" + cfg[3]->getName());

        delete cfg;
        m_game->start();
    }
}

void MainWindow::resetUI() {
    trace;
    m_lbl_status.setText("");
    m_lbl_team_ew.setText("");
    m_lbl_team_ns.setText("");
    m_lbl_tricks_ew.setText("0");
    m_lbl_tricks_ns.setText("0");
    m_lbl_score_ew.setText("0");
    m_lbl_score_ns.setText("0");
    m_lbl_bid.setText("");
    m_table.clear();
}

void MainWindow::connectContract(Contract* contract) {
    trace;
    connect(contract, SIGNAL(showCard(Card*,bool)), &m_table, SLOT(showCard(Card*,bool)));
    connect(contract, SIGNAL(placeCard(Card*,Seat,uint)), &m_table, SLOT(placeCardInTrick(Card*,Seat,uint)), Qt::QueuedConnection);
    connect(contract, SIGNAL(turnUpCard(Card*,bool)), &m_table, SLOT(turnUpCard(Card*,bool)));
    connect(contract, SIGNAL(updateState(QString)), &m_lbl_status, SLOT(setText(QString)));
    connect(contract, SIGNAL(updateEastWestTricks(QString)), &m_lbl_tricks_ew, SLOT(setText(QString)));
    connect(contract, SIGNAL(updateNorthSouthTricks(QString)), &m_lbl_tricks_ns, SLOT(setText(QString)));
    connect(contract, SIGNAL(updateBid(QString)), &m_lbl_bid, SLOT(setText(QString)));
    connect(contract, SIGNAL(animateMakeBid(Seat,Bid)), &m_table, SLOT(animateBidBubble(Seat,Bid)));
    connect(contract, SIGNAL(animateEndBidding()), &m_table, SLOT(hideBubbles()));
    connect(contract, SIGNAL(animatePlayCard(Seat,Card*,uint)), &m_table, SLOT(animatePlayCard(Seat,Card*,uint)), Qt::QueuedConnection);
    connect(contract, SIGNAL(animateCollectCards(std::vector<Card*>,Seat)), &m_table, SLOT(animateCollectCards(std::vector<Card*>,Seat)), Qt::QueuedConnection);
    connect(&m_table, SIGNAL(animationComplete()), contract, SLOT(wake()), Qt::DirectConnection);
}

void MainWindow::contractInvalid() {
    trace;
    QMessageBox::information(this, "Contract Invalid", "No-one placed a bid. The hand will be re-dealt");
    m_game->wake();
}

void MainWindow::showScores() {
    ScoreChart* s = new ScoreChart(m_game, this);
    s->exec();
    delete s;
    m_game->wake();
}

void MainWindow::threadFinished() {
    info << "thread done";
    resetUI();

    delete m_game;
    m_game = 0;

    if(m_next_action == NEW_GAME)
        newGame();

    if(m_next_action == CLOSE)
        close();

    m_next_action = NONE;
}

void MainWindow::on_actionEnd_Game_triggered()
{
    trace;
    if(m_game) {
        QMessageBox m(QMessageBox::Warning, "Game in progress", "Are you sure you wish to abort the current game?", QMessageBox::Yes | QMessageBox::No);

        if(m.exec() == QMessageBox::Yes) {
            m_game->abort();
            if(!m_biddialog->isHidden())
                m_biddialog->close();
        }
    }
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, "Five Hundred",
                       "Five Hundred version " VERSION "\n"
                       "http://fivehundred.sourceforge.net/\n\n"
                       "GNU General Public License version 3.0\n"
                       "Copyright 2011 Oliver Giles\n\n"
                       "This application contains graphics based on svg-cards (LGPL)\n"
                       "http://svg-cards.sourceforge.net/\n\n"
                       "This application is based on the Qt framework (LGPL)\n"
                       "http://qt.nokia.com/"
                       );
}

void MainWindow::on_actionExit_triggered()
{
    m_next_action = CLOSE;
    if(m_game)
        on_actionEnd_Game_triggered();
    else
        close();
}

void MainWindow::on_actionWebsite_triggered()
{
    info << "Opening " << "http://fivehundred.sourceforge.net/";
    QDesktopServices::openUrl(QUrl("http://fivehundred.sourceforge.net/"));
}
