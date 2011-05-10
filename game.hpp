#ifndef GAME_H
#define GAME_H

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
