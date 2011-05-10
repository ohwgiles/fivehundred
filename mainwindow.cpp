#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include "deck.hpp"
#include <vector>
#include <iostream>
#include <locale>
#include "player.hpp"
#include "contract.hpp"
#include "game.hpp"
#include <QResizeEvent>
#include <QMessageBox>
#include "biddialog.hpp"

#include <QPushButton>
#include <QPropertyAnimation>
#include <QGraphicsProxyWidget>

#include <Qt/qgl.h>

#include "setupplayer.hpp"
#include "newgamedialog.hpp"
#include "scorechart.hpp"
#include "human.hpp"
#include "computer.hpp"
#include "os.hpp"

#include "version.h"
#include "log.hpp"

#include <QParallelAnimationGroup>

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

    //ui->graphicsView->setViewport(new QGLWidget(QGLFormat(QGL::DirectRendering)));

    ui->graphicsView->setStyleSheet(QString("background-image: url(") + os::GFX_PATH + "table.png);");

    m_kittybuttonproxy = m_scene->addWidget(m_kittybutton);
    m_kittybutton->setVisible(false);

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

    for(QGraphicsTextItem& n: m_player_names) {
        n.setFont(font);
        n.setDefaultTextColor(QColor(Qt::white));
        m_scene->addItem(&n);
    }
    resetUI();

    show();

}

MainWindow::~MainWindow()
{
    delete m_biddialog;
    delete m_deck;
    delete m_kittybutton;
    delete ui;
}

void MainWindow::updateDisplay() {
    QSize size(ui->graphicsView->size());
    m_deck->reposition(size);
    m_player_names[int(SOUTH)].setPos(
                size.width()/2 +
                5*Card::H_OFFSET + Card::WIDTH/2,
                size.height() - Card::EDGE_OFFSET + Card::HEIGHT/2 -
                m_player_names[int(SOUTH)].boundingRect().height());
    m_player_names[int(NORTH)].setPos(
                size.width()/2 -
                5*Card::H_OFFSET - Card::WIDTH/2 - m_player_names[int(NORTH)].boundingRect().width(),
                Card::EDGE_OFFSET - Card::HEIGHT/2);
    m_player_names[int(WEST)].setPos(
                Card::EDGE_OFFSET - Card::HEIGHT/2,
                size.height()/2 + 5*Card::H_OFFSET + Card::WIDTH/2);
    m_player_names[int(EAST)].setPos(
                size.width() -
                Card::EDGE_OFFSET + Card::HEIGHT/2 - m_player_names[int(EAST)].boundingRect().width(),
                size.height()/2 - 5*Card::H_OFFSET - Card::WIDTH/2 -
                m_player_names[int(SOUTH)].boundingRect().height());
    //m_game->reposition(m_scene);
}

void MainWindow::showBidDialog(Human* player, Bidding* bids) {
    m_biddialog->show(player, bids);
}

void MainWindow::showKittyButton(bool show) {
    debug;
    m_kittybutton->setVisible(show);
    m_scene->update();
}

void MainWindow::enableKittyButton(bool en) {
    m_kittybutton->setEnabled(en);
}

void MainWindow::animatePlayCard(Card* card) {
    QPointF final = card->expectedPosition(ui->graphicsView->size(), Card::TRICK);
    QPropertyAnimation* anim = new QPropertyAnimation(card, "position");
    connect(anim, SIGNAL(finished()), this, SLOT(animationComplete_()));
    anim->setDuration(600);
    anim->setEasingCurve(QEasingCurve::OutCirc);
    anim->setKeyValueAt(0, card->pos());
    anim->setKeyValueAt(1, final);
    debug << "starting";
    anim->start(QAbstractAnimation::DeleteWhenStopped);


}

void MainWindow::animateCollectCards(std::vector<Card*> cards, Seat anc) {
    debug;

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
    debug;
    emit animationComplete();
}


void MainWindow::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);
    QSize size(ui->graphicsView->size());
    m_kittybuttonproxy->setPos(size.width()/2 - m_kittybuttonproxy->rect().width()/2, size.height()/2 - m_kittybuttonproxy->rect().height()/2);
    m_deck->reposition(size);
    m_scene->setSceneRect(0, 0, size.width(), size.height());
    //m_scene->update();
    //m_game->reposition(m_scene);
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
        m_game = new Game(deck());

        SetupPlayer** cfg = d.result();
        for(unsigned i=0; i<4; ++i) {
            if(cfg[Seat(i)]->getPlayerType() == SetupPlayer::HUMAN) {
                Human* h = new Human(Seat(i), cfg[Seat(i)]->getName());
                connect(h, SIGNAL(updateScene()), this, SLOT(updateDisplay()));
                connect(h, SIGNAL(requestBidDialog(Human*,Bidding*)), this, SLOT(showBidDialog(Human*,Bidding*)));
                connect(&biddialog(), SIGNAL(bidMade(Human*,Bid)), h, SLOT(bidMade(Human*,Bid)));
                connect(h, SIGNAL(requestKittyButtonEnabled(bool)), this, SLOT(enableKittyButton(bool)));
                connect(h, SIGNAL(requestKittyButtonVisible(bool)), this, SLOT(showKittyButton(bool)));
                connect(&kittybutton(), SIGNAL(clicked()), h, SLOT(kittyButtonClicked()));
                m_game->addPlayer(h);
            } else {
                m_game->addPlayer(new Computer(Seat(i), cfg[Seat(i)]->getName(), cfg[Seat(i)]->getScript(), m_open_hand));
            }
            m_player_names[i].setPlainText(cfg[Seat(i)]->getName());
        }

        connect(m_game, SIGNAL(sceneUpdated()), this, SLOT(updateDisplay()));
        connect(m_game, SIGNAL(newContract(Contract*)), this, SLOT(connectContract(Contract*)));
        connect(m_game, SIGNAL(finished()), this, SLOT(threadFinished()));
        connect(m_game, SIGNAL(updateNorthSouthScore(QString)), &m_lbl_score_ns, SLOT(setText(QString)));
        connect(m_game, SIGNAL(updateEastWestScore(QString)), &m_lbl_score_ew, SLOT(setText(QString)));
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
    connect(contract, SIGNAL(animatePlayCard(Card*)), this, SLOT(animatePlayCard(Card*)));
    connect(contract, SIGNAL(animateCollectCards(std::vector<Card*>,Seat)), this, SLOT(animateCollectCards(std::vector<Card*>,Seat)));
    connect(this, SIGNAL(animationComplete()), contract, SLOT(wake()), Qt::DirectConnection);
}

void MainWindow::threadFinished() {
    info << "thread done";
    resetUI();
    if(m_next_action == SHOW_SCORES) {
        ScoreChart* s = new ScoreChart(m_game, this);
        s->exec();
    }

    delete m_game;
    m_game = 0;

    if(m_next_action == NEW_GAME)
        newGame();

    m_next_action = NONE;
}

void MainWindow::on_actionEnd_Game_triggered()
{
    trace;
    if(m_game) {
        QMessageBox m(QMessageBox::Warning, "Game in progress", "Are you sure you wish to abort the current game?", QMessageBox::Yes | QMessageBox::No);

        if(m.exec() == QMessageBox::Yes)
            m_game->abort();
    }
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, "Five Hundred",
                       "Five Hundred version " VERSION "\n\n"
                       "GNU General Public License version 2.0\n"
                       "Copyright 2011 Oliver Giles\n"
                       );
}
