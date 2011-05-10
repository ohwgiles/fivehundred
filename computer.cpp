#include "computer.hpp"
#include "card.hpp"
#include "trick.hpp"
#include "bidding.hpp"
#include "luahelper.hpp"
#include "log.hpp"

#include <algorithm>

template<>
bool lua_cmp<Bid>(lua_State*, const Bid& op1, const Bid& op2) { return op1 < op2; }

template<>
bool lua_cmp<Card>(lua_State* L, const Card& op1, const Card& op2) {
    Computer* c = static_cast<Computer*>(lua_touserdata(L, lua_upvalueindex(1)));
    Trick::CompareInTrick comp(c->m_trumps, c->m_lead);
    return comp(&op1, &op2);
}

template<>
int equality<Player>(lua_State* L) {
    trace;
    lua_pushboolean(L, *lua_extract<Player>(L) == *lua_extract<Player>(L));
    return 1;
}

template<>
void register_additional_metas<Bid>(lua_State* L, int tbl) {
    trace;
    lua_pushliteral(L, "__call");
    lua_pushcclosure(L, [](lua_State* L) -> int {

        if(lua_isnumber(L, -2)) {
            const char* suit_str = luaL_checklstring(L, -1, 0);
            std::stringstream ss(suit_str);
            Suit suit;
            ss >> suit;
            if(ss.bad()) error << "Could not deserialise Suit: " << suit_str;
            lua_pop(L, 1);
            int numtricks = luaL_checkinteger(L, -1);
            lua_pop(L, 1);
            Bid* b = new Bid(suit, numtricks);
            pushClass(L, *b);
        } else {
            const char* type_str = luaL_checklstring(L, -1, 0);
            std::stringstream ss(type_str);
            Bid* b = new Bid();
            ss >> *b;
            pushClass(L, *b);
        }
        return 1;
    }, 0);

    lua_settable(L, tbl);
    lua_pushliteral(L, "__gc");

    lua_pushcclosure(L, [](lua_State* L) -> int {
        Bid* b = lua_extract<Bid>(L);
        delete b;
        return 0;
    }, 0);

    lua_settable(L, tbl);
}

static int computerLog(lua_State* L) {
    trace;
    std::string name_ = ((Computer*)lua_touserdata(L, lua_upvalueindex(1)))->name.toStdString();
    const char* name = name_.c_str();
    while(lua_gettop(L) > 0) {
        if(lua_isstring(L, 1)) {
            user << lua_tostring(L, 1);
        } else if(lua_isnumber(L, 1)) {
            user << lua_tonumber(L, 1);
        } else if(lua_isboolean(L, 1)) {
            user << (lua_toboolean(L, 1) ? "True" : "False");
        } else if(lua_isuserdata(L, 1)) {
            lua_getmetatable(L, 1);
            lua_getfield(L, -1, "__tostring");
            lua_pushvalue(L, -3);
            lua_call(L, 1, 1);
            user << lua_tostring(L, -1);
            lua_pop(L, 2);
        } else {
            user << "Unprintable type: " << luaL_typename(L, 1);
        }
        lua_remove(L, 1);
    }
    return 0;
}

Computer::Computer(Seat pos, const QString& name, const QString& script, bool play_open_hand) :
        Player(pos, name),
        m_trumps(Suit::NONE),
        m_lead(Suit::NONE),
        L(0),
        luareg(0),
        m_play_open_hand(play_open_hand)
{
    trace;
    L = lua_open();


    luaL_openlibs(L);

    lua_pushliteral(L, "print");
    lua_pushlightuserdata(L, (void*)this);
    lua_pushcclosure(L, &computerLog, 1);
    lua_settable(L, LUA_GLOBALSINDEX);


    luareg = new LuaRegistration(L, this);
/*
    luareg->add_class<Suit>(
                "SPADES", "Spades",
                "CLUBS", "Clubs",
                "DIAMONDS", "Diamonds",
                "HEARTS", "Hearts",
                "NONE", "No Trumps"
                );*/

    luareg->add_class<Bid>(
                "PASS", Bid::PASS,
                "NORMAL", Bid::NORMAL,
                "CLOSED_MISERE", Bid::CLOSED_MISERE,
                "OPEN_MISERE", Bid::OPEN_MISERE,
                "suit", [](lua_State* L, Computer*) {
                    //lua_pushnumber(L, lua_extract<Bid>(L)->suit());
                    //pushClass(L, lua_extract<Bid>(L)->suit());
                    std::ostringstream ss; ss << lua_extract<Bid>(L)->suit();
                    lua_pushstring(L, ss.str().c_str());
                },
                "type", [](lua_State* L, Computer*) {
                    lua_pushnumber(L, lua_extract<Bid>(L)->type());
                },
                "tricks", [](lua_State* L, Computer*) {
                    lua_pushnumber(L, lua_extract<Bid>(L)->tricks());
                }
                );

    luareg->add_class<Card>(
                "TWO", Card::TWO,
                "THREE", Card::THREE,
                "FOUR", Card::FOUR,
                "FIVE", Card::FIVE,
                "SIX", Card::SIX,
                "SEVEN", Card::SEVEN,
                "EIGHT", Card::EIGHT,
                "NINE", Card::NINE,
                "TEN", Card::TEN,
                "JACK", Card::JACK,
                "QUEEN", Card::QUEEN,
                "KING", Card::KING,
                "ACE", Card::ACE,
                "LEFT_BOWER", Card::LEFT_BOWER,
                "RIGHT_BOWER", Card::RIGHT_BOWER,
                "JOKER", Card::JOKER,
                "isTrump", [](lua_State* L, Computer* computer) {
                    lua_pushnumber(L, lua_extract<Card>(L)->isTrump(computer->m_trumps));
                },
                "suit", [](lua_State* L, Computer* computer) {
                    //lua_pushnumber(L, lua_extract<Card>(L)->suit(computer->m_trumps));
                    //pushNewClass(L, lua_extract<Card>(L)->suit(computer->m_trumps));
                    std::ostringstream ss; ss << lua_extract<Card>(L)->suit(computer->m_trumps);
                    lua_pushstring(L, ss.str().c_str());

                },
                "value", [](lua_State* L, Computer* computer) {
                    lua_pushnumber(L, lua_extract<Card>(L)->value(computer->m_trumps));
                }
                );

    luareg->add_class<Player>("hand", LuaTable());

    luaL_loadfile(L, script.toAscii());
    lua_pcall(L, 0, 0, 0);
}

void Computer::configure() {
    trace;
    lua_getglobal(L, "Player");
    pushKeyValue(L, "_me", [=](){ pushClass(L, *this); } );
    pushKeyValue(L, "left", [=](){ pushClass(L, *next); } );
    pushKeyValue(L, "partner", [=](){ pushClass(L, *(next->next)); } );
    pushKeyValue(L, "right", [=](){ pushClass(L, *(next->next->next)); } );
    lua_pop(L, 1);
}

Computer::~Computer() {
    trace;
    delete luareg;
    lua_close(L);
}


void Computer::bidWon(const Bidding* bidlist, const Player* winner) {
    debug;
    m_trumps = bidlist->winner().bid.suit();
    callLua(L, __func__, 2, [&]() {
        debug;
        pushBidList(bidlist);
        pushClass(L, *winner);
    });
}

void Computer::handDealt() {
    debug;
    for(Card* c: hand)
        c->setFaceUp(m_play_open_hand);
    layoutHand();
    registerHand();
    callLua(L, __func__);
}

void Computer::registerHand() {
    trace;
    lua_getglobal(L, "Player");
    pushKeyValue(L, "hand", [&]() {
        pushList(L, hand.size(), [&](int i){
            pushClass(L, *hand.at(i));
        });
    });
    lua_pop(L, 1);
}


Card* Computer::yourTurnToPlay(const Trick* trick) {
    debug;
    Card* myCard;
    if(trick->size() > 0)
        m_lead = trick->card(0)->suit(m_trumps);
    callLua(L, __func__, 1, [&](){
        pushList(L, trick->size(), [&](int i){
            lua_newtable(L);
            pushKeyValue(L, "player", [&](){ pushClass(L, *trick->player(i)); });
            pushKeyValue(L, "card", [&](){ pushClass(L, *trick->card(i)); });
        });
    }, 1, [&](){
        Card* c = lua_extract<Card>(L);
        debug << "Extracted card: " << *c;
        if(std::find(hand.begin(), hand.end(), c) != hand.end()) {
			if(this->cardValid(trick, m_trumps, *c)) {
				hand.erase(std::find(hand.begin(), hand.end(), c));
				registerHand();
				//trick->playCard(this, c);
				myCard = c;
			} else
				fatal(error << "Tried to play an illegal card - run with debug for why");
        } else
            fatal(error<<"Tried to play a non-existant card");
    });

    m_lead = Suit::NONE;
    return myCard;
}

void Computer::pushBidList(const Bidding *bidlist) {
    trace;
    pushList(L, bidlist->count(), [&](int i){
        lua_newtable(L);
        pushKeyValue(L, "player", [&](){
            lua_getglobal(L, "Player");
            if(bidlist->at(i).player == this)
                lua_getfield(L, lua_gettop(L), "_me");
            else if(bidlist->at(i).player == next)
                lua_getfield(L, lua_gettop(L), "left");
            else if(bidlist->at(i).player == next->next)
                lua_getfield(L, lua_gettop(L), "partner");
            else if(bidlist->at(i).player == next->next->next)
                lua_getfield(L, lua_gettop(L), "right");
            else
                fatal(error << "Invalid player");
            lua_remove(L, -2);
        });
        pushKeyValue(L, "bid", [&](){
            pushNewClass(L, bidlist->at(i).bid);
        });
    });
}

Bid Computer::yourTurnToBid(const Bidding* bidlist) {
    debug;
    Bid myBid;
    callLua(L, __func__, 1, [&]() {
        pushBidList(bidlist);
    }, 1, [&]() {
        Bid* b = lua_extract<Bid>(L);
        if(!(*b == Bid::PASS) && bidlist->hasWinner() && *b < bidlist->winner().bid)
            error << "You tried to bid " << *b << " but " << bidlist->winner().player << " had already bid " << bidlist->winner().bid;
//bidlist->bid(this, Bid(*b));
myBid = *b;
/*
        if(!lua_isnumber(L, -2))
            error << "first return value of yourTurnToBid not a number";

        if(!lua_isnumber(L,-1)) {
            Bid::Type bidtype = Bid::Type(luaL_checkint(L, -2));
            switch(bidtype) {
            case Bid::PASS: case Bid::CLOSED_MISERE: case Bid::OPEN_MISERE:
                bidlist->bid(this, bidtype);
                break;
            default:
                error << __func__ << " returned Bid type that was not one of Pass, Closed Misere or Open Misere";
            }
        } else {
            int numTricks = luaL_checkint(L, -2);
            Suit::SuitEnum bidSuit = Suit::SuitEnum(luaL_checkint(L, -1));
            if(numTricks < 6 || numTricks > 10) error << "Player " << name << " tried to bid " << numTricks << " tricks";
            switch(bidSuit) {
                case Suit::CLUBS: case Suit::SPADES: case Suit::DIAMONDS: case Suit::HEARTS: case Suit::NONE:
                    bidlist->bid(this, Bid(Suit(bidSuit), numTricks));
                    break;
                default:
                    error << "Player " << name << " tried to bid an invalid suit";
            }
        }
        lua_pop(L, 2);*/
     });
return myBid;
}

Hand Computer::yourTurnToSelectKitty(const Hand& kitty) {
    debug;
    std::vector<Card*> discards(3);
    callLua(L, __func__, 1, [&](){
            pushList(L, 3, [&](int i){
                     pushClass(L, *kitty.at(i));
            });
    }, 1, [&](){
        if(lua_istable(L, -1)) {
            int discardtable = lua_gettop(L);

            for(int i=0; i<3; ++i) {
                lua_pushnumber(L, i+1);
                lua_gettable(L, discardtable);
                Card** c = static_cast<Card**>(luaL_checkudata(L, -1, Card::className));
                if(!c)
                    fatal(error << "return value of " << __func__ << " did not contain a Card at index " << i+1);
                Hand::iterator it;
                if((it = std::find(hand.begin(), hand.end(), *c)) != hand.end())
                    hand.erase(it);
                else {
                    if(std::find(kitty.begin(), kitty.end(), *c) == kitty.end() || std::find(discards.begin(), discards.end(), *c) != discards.end())
                        fatal(error << "You tried to discard an invalid card or a card you already discarded");
                }
                discards[i] = *c;
                lua_pop(L, 1);
            }
            for(Hand::const_iterator it = kitty.begin(); it != kitty.end(); ++it) {
                if(std::find(discards.begin(), discards.end(), *it) == discards.end()) {
                    (*it)->setFaceUp(m_play_open_hand);
                    hand.push_back(*it);
                }
            }
            // update player hand
            registerHand();
            layoutHand();
            //emit this->kittyChosen(discards);

        } else
            error << __func__ << " must return table of discards";
        lua_pop(L, 1);
    });
    return discards;
}

void Computer::trickWon(const Trick& trick, const Player* winner) {
    debug;
    callLua(L, __func__, 2, [&](){
        pushList(L, trick.size(), [&](int i){
            lua_newtable(L);
            pushKeyValue(L, "player", [&](){ pushClass(L, *trick.player(i)); });
            pushKeyValue(L, "card", [&](){ pushClass(L, *trick.card(i)); });
        });
        pushClass(L, *winner);
    });
    //emit acknowledgeTrick(this);
}

