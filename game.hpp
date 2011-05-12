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

class Contract;
class Player;
class Deck;

class Game : public QThread {
    Q_OBJECT

public:
    Game(Deck& deck);
    virtual ~Game();

    void abort();

    void addPlayer(Player* new_player);

    virtual void run();

    void updateTricksWon(int northsouth, int eastwest);
    void updateScene() { emit sceneUpdated(); }

signals:
    void sceneUpdated();
    void updateNorthSouthScore(QString);
    void updateEastWestScore(QString);
    void newContract(Contract* current);

private:
    void updateScores(int northsouth, int eastwest);
    Deck& m_deck;

    friend class ScoreChart;

    Player* m_players[4];
    int m_num_players;
    Player* m_first_player;
    std::vector<Contract*> m_contracts;

    std::vector<std::pair<int,int> > m_scores;
    std::pair<int, int> m_scores_sum;
};
#endif // GAME_H
