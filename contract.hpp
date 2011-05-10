#ifndef CONTRACT_HPP
#define CONTRACT_HPP

#include <vector>
#include <QObject>
#include <QGraphicsScene>
#include <QLabel>
#include <QMutex>
#include <QWaitCondition>

#include "player.hpp"
#include "trick.hpp"
#include "bidding.hpp"

class Player;

class Contract : public QObject {
    Q_OBJECT
public:
    Contract(Player* first_player);
    ~Contract();

    enum ExitState { SUCCESS, NOBIDS, REDEAL, ABORT };
    ExitState start();

    const Bidding::Pair& bidWinner() const { return m_bids.winner(); }
    bool successful() const;
    int tricksWon(const Player* player) const;

    std::vector<Card*> m_kitty;
    void abort();

signals:
    void sceneUpdated();
    void updateState(QString);
    void updateBid(QString);
    void updateEastWestTricks(QString);
    void updateNorthSouthTricks(QString);
    void animatePlayCard(Card*);
    void animateCollectCards(std::vector<Card*>, Seat);

protected slots:
    void wake();

protected:
    bool m_abort;
    std::vector<Trick*> m_tricks;
    Suit m_trumps;
    Bidding m_bids;
    Player* m_current_player;
    QMutex m_mutex;
    QWaitCondition m_wait;
    int m_northsouth_tricks;
    int m_eastwest_tricks;
};

#endif // CONTRACT_HPP
