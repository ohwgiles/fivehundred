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
#include "table.hpp"
#include "bid.hpp"

namespace Ui {
    class MainWindow;
}

// Forward Declarations
class Game;
class Human;
class Bidding;
class Contract;
class BidDialog;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(bool open_hand, QWidget *parent = 0);
    virtual ~MainWindow();

protected:
    void resizeEvent(QResizeEvent* event);
    void resetUI(); //!< Hide all cards, set all scores to zero, etc

protected slots:
    void threadFinished(); //!< The worker thread has completed, i.e. Game over
    void connectContract(Contract* contract);
    void showScores();
    void contractInvalid();

private slots:
    void on_actionNew_Game_triggered();
    void on_actionEnd_Game_triggered();
    void on_actionAbout_triggered();

    void on_actionExit_triggered();

    void on_actionWebsite_triggered();

private:
    void newGame();

    Ui::MainWindow *ui;
    QPushButton* m_kittybutton;
    BidDialog* m_biddialog;

    Game* m_game;
    Table m_table;

    bool m_open_hand; //!< Whether the Computer players play open hand

    enum Action { NONE, NEW_GAME, CLOSE };
    Action m_next_action; //!< Allow functionality after the worker thread has completed

    QLabel m_lbl_status;
    QLabel m_lbl_team_ew;
    QLabel m_lbl_team_ns;
    QLabel m_lbl_tricks_ew;
    QLabel m_lbl_tricks_ns;
    QLabel m_lbl_score_ew;
    QLabel m_lbl_score_ns;
    QLabel m_lbl_bid;

};


#endif // MAINWINDOW_HPP
