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
    m_game(0),
    m_scene(new QGraphicsScene(this)),
    m_deck(new Deck()),
    m_biddialog(new BidDialog(this)),
    m_kittybutton(new QPushButton("Discard Kitty", 0)),
    m_kittybuttonproxy(0),
    m_open_hand(open_hand)
{
    ui->setupUi(this);

    ui->graphicsView->setScene(m_scene);

    for(Card* c: *m_deck)
        m_scene->addItem(c);

    // It would be nice to use direct rendering some time maybe?
    // QGLWidget* gl = new QGLWidget(QGLFormat(QGL::DirectRendering));
    //ui->graphicsView->setViewport(gl);
    ui->graphicsView->setStyleSheet(QString("background-image: url('") + os::GFX_PATH + "table.gif');");

    // Add all extra widgets to scene
    m_kittybuttonproxy = m_scene->addWidget(m_kittybutton);
    ui->statusBar->addWidget(&m_lbl_status, 1);
    ui->statusBar->addWidget(&m_lbl_team_ns);
    ui->statusBar->addWidget(&m_lbl_tricks_ns);
    ui->statusBar->addWidget(&m_lbl_score_ns);
    ui->statusBar->addWidget(&m_lbl_team_ew);
    ui->statusBar->addWidget(&m_lbl_tricks_ew);
    ui->statusBar->addWidget(&m_lbl_score_ew);
    m_lbl_bid.setAlignment(Qt::AlignRight);
    ui->statusBar->addWidget(&m_lbl_bid, 1);

    QFont font;
    font.setPointSize(10);
    font.setStyleHint(QFont::SansSerif);

    // Add player names to scene
    for(QGraphicsTextItem& n: m_player_names) {
        n.setFont(font);
        n.setDefaultTextColor(QColor(Qt::white));
        m_scene->addItem(&n);
    }

    resetUI();

    show();
    // Still not quite sure why we have to do this
    this->resize(m_scene->sceneRect().size().toSize());
}

MainWindow::~MainWindow() {
    trace;
    delete m_biddialog;
    delete m_deck;
    delete m_kittybutton;
    delete ui;
}

void MainWindow::updateDisplay() {
    trace;
    QSize size(ui->graphicsView->size());
    m_deck->reposition(size);
    repositionPlayerNames();
}

void MainWindow::showBidDialog(Human* player, Bidding* bids) {
    trace;
    m_biddialog->show(player, bids);
}

void MainWindow::showKittyButton(bool show) {
    trace;
    m_kittybutton->setVisible(show);
    //m_scene->update();
}

void MainWindow::enableKittyButton(bool en) {
    trace;
    m_kittybutton->setEnabled(en);
}

void MainWindow::animatePlayCard(Card* card) {
    trace;
    QPointF final = card->expectedPosition(ui->graphicsView->size(), Card::TRICK);
    QPropertyAnimation* anim = new QPropertyAnimation(card, "position");
    connect(anim, SIGNAL(finished()), this, SLOT(animationComplete_()));
    anim->setDuration(600);
    anim->setEasingCurve(QEasingCurve::OutCirc);
    anim->setKeyValueAt(0, card->pos());
    anim->setKeyValueAt(1, final);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void MainWindow::animateCollectCards(std::vector<Card*> cards, Seat anc) {
    trace;
    QParallelAnimationGroup* anim_group = new QParallelAnimationGroup();
    for(Card* c : cards) {
        QPropertyAnimation* pos = new QPropertyAnimation(c, "position");
        pos->setDuration(1000);
        pos->setKeyValueAt(0, c->pos());
        pos->setKeyValueAt(0.5, c->pos());
        QPointF final;
        switch(anc) {
        case SOUTH:
            final = QPointF(ui->graphicsView->width()/2, ui->graphicsView->height());
            break;
        case WEST:
            final = QPointF(0, ui->graphicsView->height()/2);
            break;
        case EAST:
            final = QPointF(ui->graphicsView->width(), ui->graphicsView->height()/2);
            break;
        case NORTH:
            final = QPointF(ui->graphicsView->width()/2, 0);
            break;
        default:
            fatal(error<<"Invalid anchor point");
        }

        pos->setKeyValueAt(1, final);
        anim_group->addAnimation(pos);

        QPropertyAnimation* opacity = new QPropertyAnimation(c, "opacity");
        opacity->setDuration(1000);
        opacity->setKeyValueAt(0, 1);
        opacity->setKeyValueAt(0.5, 1);
        opacity->setKeyValueAt(0.8, 0);
        opacity->setKeyValueAt(1, 0);
        anim_group->addAnimation(opacity);
    }
    connect(anim_group, SIGNAL(finished()), this, SLOT(animationComplete_()));
    anim_group->start(QAbstractAnimation::DeleteWhenStopped);
}

void MainWindow::animationComplete_() {
    trace;
    emit animationComplete();
}

void MainWindow::repositionPlayerNames() {
    QSize size(ui->graphicsView->size());
    m_player_names[int(SOUTH)].setPos(
                size.width()/2 +
                4.5*Card::H_OFFSET + Card::WIDTH/2 - m_player_names[int(SOUTH)].boundingRect().width(),
                size.height() - Card::EDGE_OFFSET + Card::HEIGHT/2);
    m_player_names[int(NORTH)].setPos(
                size.width()/2 -
                4.5*Card::H_OFFSET - Card::WIDTH/2,
                Card::EDGE_OFFSET - Card::HEIGHT/2 - m_player_names[int(NORTH)].boundingRect().height());
    m_player_names[int(WEST)].setPos(
                Card::EDGE_OFFSET - Card::HEIGHT/2,
                size.height()/2 + 4.5*Card::H_OFFSET + Card::WIDTH/2);
    m_player_names[int(EAST)].setPos(
                size.width() -
                Card::EDGE_OFFSET + Card::HEIGHT/2 - m_player_names[int(EAST)].boundingRect().width(),
                size.height()/2 - 4.5*Card::H_OFFSET - Card::WIDTH/2 -
                m_player_names[int(SOUTH)].boundingRect().height());

}

void MainWindow::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);
    QSize size(ui->graphicsView->size());
    m_scene->setSceneRect(0, 0, size.width(), size.height());
    m_kittybuttonproxy->setPos(size.width()/2 - m_kittybuttonproxy->rect().width()/2, size.height()/2 - m_kittybuttonproxy->rect().height()/2);
    m_deck->reposition(size);
    repositionPlayerNames();
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
        m_game = new Game(m_deck);

        SetupPlayer** cfg = d.result();
        for(unsigned i=0; i<4; ++i) {
            if(cfg[Seat(i)]->getPlayerType() == SetupPlayer::HUMAN) {
                Human* h = new Human(Seat(i), cfg[Seat(i)]->getName());
                connect(h, SIGNAL(updateScene()), this, SLOT(updateDisplay()));
                connect(h, SIGNAL(requestBidDialog(Human*,Bidding*)), this, SLOT(showBidDialog(Human*,Bidding*)));
                connect(m_biddialog, SIGNAL(bidMade(Human*,Bid)), h, SLOT(bidMade(Human*,Bid)));
                connect(h, SIGNAL(requestKittyButtonEnabled(bool)), this, SLOT(enableKittyButton(bool)));
                connect(h, SIGNAL(requestKittyButtonVisible(bool)), this, SLOT(showKittyButton(bool)));
                connect(m_kittybutton, SIGNAL(clicked()), h, SLOT(kittyButtonClicked()));
                m_game->addPlayer(h);
            } else {
                m_game->addPlayer(new Computer(Seat(i), cfg[Seat(i)]->getName(), cfg[Seat(i)]->getScript(), m_open_hand));
            }
            m_player_names[i].setPlainText(cfg[Seat(i)]->getName());
        }

        connect(m_game, SIGNAL(sceneUpdated()), this, SLOT(updateDisplay()));
        connect(m_game, SIGNAL(newContract(Contract*)), this, SLOT(connectContract(Contract*)), Qt::DirectConnection);
        connect(m_game, SIGNAL(finished()), this, SLOT(threadFinished()));
        connect(m_game, SIGNAL(updateNorthSouthScore(QString)), &m_lbl_score_ns, SLOT(setText(QString)));
        connect(m_game, SIGNAL(updateEastWestScore(QString)), &m_lbl_score_ew, SLOT(setText(QString)));
        connect(m_game, SIGNAL(contractComplete()), this, SLOT(showScores()));

        m_lbl_team_ns.setText(cfg[0]->getName() + "/" + cfg[2]->getName());
        m_lbl_team_ew.setText(cfg[1]->getName() + "/" + cfg[3]->getName());

        delete cfg;
        m_game->start();
        m_next_action = SHOW_SCORES;
    }
}

void MainWindow::resetUI() {
    trace;
    m_deck->hide();
    m_lbl_status.setText("");
    m_lbl_team_ew.setText("");
    m_lbl_team_ns.setText("");
    m_lbl_tricks_ew.setText("0");
    m_lbl_tricks_ns.setText("0");
    m_lbl_score_ew.setText("0");
    m_lbl_score_ns.setText("0");
    m_lbl_bid.setText("");
    m_kittybutton->hide();
    for(QGraphicsTextItem& n: m_player_names)
        n.setPlainText("");
    updateDisplay();
}

void MainWindow::connectContract(Contract* contract) {
    trace;
    connect(contract, SIGNAL(sceneUpdated()), this, SLOT(updateDisplay()));
    connect(contract, SIGNAL(updateState(QString)), &m_lbl_status, SLOT(setText(QString)));
    connect(contract, SIGNAL(updateEastWestTricks(QString)), &m_lbl_tricks_ew, SLOT(setText(QString)));
    connect(contract, SIGNAL(updateNorthSouthTricks(QString)), &m_lbl_tricks_ns, SLOT(setText(QString)));
    connect(contract, SIGNAL(updateBid(QString)), &m_lbl_bid, SLOT(setText(QString)));
    connect(contract, SIGNAL(animatePlayCard(Card*)), this, SLOT(animatePlayCard(Card*)), Qt::QueuedConnection);
    connect(contract, SIGNAL(animateCollectCards(std::vector<Card*>,Seat)), this, SLOT(animateCollectCards(std::vector<Card*>,Seat)), Qt::QueuedConnection);
    connect(this, SIGNAL(animationComplete()), contract, SLOT(wake()), Qt::DirectConnection);
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
    if(m_next_action == SHOW_SCORES) {
    }

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
            m_next_action = NONE;
            m_game->abort();
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
