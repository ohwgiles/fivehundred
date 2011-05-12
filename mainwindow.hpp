#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP
/*!
  \file mainwindow.hpp
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
#include <QMainWindow>
#include <QLabel>
#include <QGraphicsTextItem>
#include "seat.hpp"

namespace Ui {
    class MainWindow;
}

// Forward Declarations
class QGraphicsScene;
class Deck;
class BidDialog;
class QAbstractListModel;
class QPushButton;
class Human;
class Card;
class Bidding;
class Contract;
class Game;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(bool open_hand, QWidget *parent = 0);
    virtual ~MainWindow();

protected:
    void resizeEvent(QResizeEvent* event);
    void resetUI(); //!< Hide all cards, set all scores to zero, etc
    void repositionPlayerNames();

signals:
    void reposition();
    void animationComplete(); //!< Emitted when the GUI has finished animating an object

protected slots:
    void updateDisplay(); //!< The worker thread can cause the GUI to be updated
    void showBidDialog(Human* player, Bidding* bids);
    void showKittyButton(bool); //!< Human players can request the kitty button
    void enableKittyButton(bool); //!< Human players can request the kitty button
    void animatePlayCard(Card*); //!< the Contract requests animations
    void animateCollectCards(std::vector<Card*>, Seat); //!< the Contract requests animations
    void animationComplete_(); //!< Internal callback when the animation is complete
    void threadFinished(); //!< The worker thread has completed, i.e. Game over
    void connectContract(Contract* contract);

private slots:
    void on_actionNew_Game_triggered();
    void on_actionEnd_Game_triggered();
    void on_actionAbout_triggered();

private:
    void newGame();

    Ui::MainWindow *ui;

    Game* m_game;
    QGraphicsScene* m_scene;
    Deck* m_deck;
    BidDialog* m_biddialog;
    QPushButton* m_kittybutton;
    QGraphicsProxyWidget* m_kittybuttonproxy;

    bool m_open_hand; //!< Whether the Computer players play open hand

    enum Action { NONE, NEW_GAME, SHOW_SCORES };
    Action m_next_action; //!< Allow functionality after the worker thread has completed

    QLabel m_lbl_status;
    QLabel m_lbl_team_ew;
    QLabel m_lbl_team_ns;
    QLabel m_lbl_tricks_ew;
    QLabel m_lbl_tricks_ns;
    QLabel m_lbl_score_ew;
    QLabel m_lbl_score_ns;
    QLabel m_lbl_bid;

    QGraphicsTextItem m_player_names[4];
};


#endif // MAINWINDOW_HPP
