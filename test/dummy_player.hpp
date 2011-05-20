#ifndef DUMMY_PLAYER_HPP
#define DUMMY_PLAYER_HPP

#include "player.hpp"

class DummyPlayer : public Player {
public:
    DummyPlayer() :
        Player(SOUTH, "Dummy") {}

    virtual void handDealt() {}
    virtual Bid yourTurnToBid(const Bidding*) { return Bid(); }
    virtual void bidWon(const Bidding*, const Player*) {}
    virtual Hand yourTurnToSelectKitty(const Hand& h) { return h; }
    virtual Card* yourTurnToPlay(const Trick*) { return NULL; }
    virtual void trickWon(const Trick&, const Player*) {}
};

#endif // DUMMY_PLAYER_HPP
