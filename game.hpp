#ifndef GAME_H
#define GAME_H
/*!
  \file game.hpp
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
#include "game.hpp"
#include <vector>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>

// Forward Declarations
class Contract;
class Player;
class Deck;

/*!
  \class Game
  \brief Maintains players, contracts, and scores

  The Game is a worker thread spawned using QThread::start() or a non-interactive
  game called using run().

  Before starting a game you must add four players using the addPlayer method.

  Game maintains a list of contracts, adding more until the score has reached positive
  or negative 500. It handles scoring and dealing new hands at the start of each
  contract, ensuring each contract begins play from a subsequent player position.
*/
class Game : public QThread {
    Q_OBJECT

public:
    Game(Deck* deck);
    virtual ~Game();

    void abort(); //!< Stop the current game
    void wake();
    void addPlayer(Player* new_player); //!< Use this function to add four players to the game
    virtual void run();

signals:
    // Signals connect to the GUI thread
    void sceneUpdated();
    void updateNorthSouthScore(QString);
    void updateEastWestScore(QString);
    void newContract(Contract* current);
    void contractComplete();
    void contractInvalidated();

private:
    void updateScores(int northsouth, int eastwest);
    Deck* m_deck;

    Player* m_players[4];
    int m_num_players;
    Player* m_first_player;

    std::vector<Contract*> m_contracts;

    // Scores
    std::vector<std::pair<int,int> > m_scores;
    std::pair<int, int> m_scores_sum;

    QMutex m_mutex;
    QWaitCondition m_wait;

    //! The ScoreChart needs access to the individual contracts and scores
    friend class ScoreChart;
};
#endif // GAME_H
