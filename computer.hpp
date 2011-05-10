#ifndef COMPUTER_HPP
#define COMPUTER_HPP

#include "player.hpp"

struct lua_State;
struct LuaRegistration;

class Computer : public Player
{
public:
    Computer(Seat pos, const QString& name, const QString& script, bool play_open_hand);
    virtual ~Computer();
    virtual void configure();
    virtual void handDealt();

    virtual Card* yourTurnToPlay(const Trick*);
    virtual Bid yourTurnToBid(const Bidding* bidlist);
    virtual void bidWon(const Bidding* bidlist, const Player* winner);
    virtual Hand yourTurnToSelectKitty(const Hand& kitty);

    virtual void trickWon(const Trick& trick, const Player* winner);

    Suit m_trumps;
    Suit m_lead;

protected:

    void pushBidList(const Bidding* bidlist);
    void registerHand();

    lua_State* L;
    LuaRegistration* luareg;

    Bidding* m_current_bidlist;
    Trick* m_current_trick;

    bool m_play_open_hand;
};

#endif // COMPUTER_HPP
