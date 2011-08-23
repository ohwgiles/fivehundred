#include "computer.hpp"
/*!
  \file computer.cpp
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
#include "card.hpp"
#include "trick.hpp"
#include "bidding.hpp"
#include "lupha.hpp"
#include <algorithm>
#include "log.hpp"

Computer::Computer(Seat pos, const QString& name, const QString& script, bool play_open_hand) :
        Player(pos, name),
        Scriptable(script),
        m_play_open_hand(play_open_hand)
{
    trace;
}

void Computer::testCallFunc(const char* func) {
    trace;
    info;
    callLua(L, func);
}

void Computer::configure() {
    trace;
    //lua_getglobal(L, "Player");
    lua_pushstring(L, "ME");
    lua_pushstring(L, name().toLocal8Bit());
    lua_settable(L, LUA_GLOBALSINDEX);
    lua_pushstring(L, "LEFT");
    lua_pushstring(L, next->name().toLocal8Bit());
    lua_settable(L, LUA_GLOBALSINDEX);
    lua_pushstring(L, "PARTNER");
    lua_pushstring(L, (next->next->name().toLocal8Bit()));
    lua_settable(L, LUA_GLOBALSINDEX);
    lua_pushstring(L, "RIGHT");
    lua_pushstring(L, (next->next->next->name().toLocal8Bit()));
    lua_settable(L, LUA_GLOBALSINDEX);
}

Computer::~Computer() {
    trace;
    lua_close(L);
}


void Computer::bidWon(const Bidding* bidlist, const Player* winner) {
    debug;
    m_trumps = bidlist->winner().bid.suit();
    callLua(L, __func__, 2, [&]() {
        debug;
        pushBidList(bidlist);
        lua_pushstring(L, winner->name().toLocal8Bit());
    });
}

void Computer::handDealt() {
    debug;
    for(Card* c: hand)
        //c->setFaceUp(m_play_open_hand);
        emit turnUpCard(c, m_play_open_hand);
    layoutHand();
    registerHand();
    callLua(L, __func__);
}

void Computer::registerHand() {
    trace;
    //lua_getglobal(L, "Player");
   // if(lua_gettop(L) != LUA_GLOBALSINDEX)
   //     fatal(error<<"not at root!");
    lua_pushstring(L, "HAND");
    //pushKeyValue(L, LUA_GLOBALSINDEX, "hand", [&]() {
        pushList(L, hand.size(), [&](int i){
            pushClass<SimpleCard>(L, *hand.at(i));
        });

    //});
    lua_settable(L, LUA_GLOBALSINDEX);
    //lua_pop(L, 1);
}


Card* Computer::yourTurnToPlay(const Trick* trick) {
    debug;
    Card* myCard;
    if(trick->size() > 0)
        m_lead = trick->card(0)->suit(m_trumps);
    else
        m_lead = Suit::NONE;
    callLua(L, __func__, 1, [&](){
        pushList(L, trick->size(), [&](int i){
            lua_newtable(L);
            trace;
            lua_pushstring(L, "player");
    trace << lua_gettop(L);
            lua_pushstring(L, trick->player(i)->name().toLocal8Bit());
            trace << lua_gettop(L);
            lua_settable(L, -3);
            trace;
            lua_pushstring(L, "card");
            trace;
            pushClass<SimpleCard>(L, *trick->card(i));
            lua_settable(L, -3);
        });
    }, 1, [&](){
        debug;
        SimpleCard* c = lua_extract<SimpleCard>(L);
        debug << "Extracted card: " << *c;
        Hand::iterator it = findCard(hand, c);
        if(it != hand.end()) {
                        if(this->cardValid(trick, m_trumps, **it)) {
                                myCard = *it;
                                hand.erase(it);
				registerHand();
				//trick->playCard(this, c);
			} else
				fatal(error << "Tried to play an illegal card - run with debug for why");
        } else
            fatal(error<<"Tried to play a non-existant card: " << *c);
    });

    m_lead = Suit::NONE;
    return myCard;
}

void Computer::pushBidList(const Bidding *bidlist) {
    trace;
    pushList(L, bidlist->count(), [&](int i){
        lua_newtable(L);
        lua_pushstring(L, "player");
    lua_pushstring(L, bidlist->at(i).player->name().toLocal8Bit());
        /*
            if(bidlist->at(i).player == this)
                lua_getglobal(L, "me");
            else if(bidlist->at(i).player == next)
                lua_getglobal(L, "left");
            else if(bidlist->at(i).player == next->next)
                lua_getglobal(L, "partner");
            else if(bidlist->at(i).player == next->next->next)
                lua_getglobal(L, "right");
            else
                fatal(error << "Invalid player");*/

            lua_settable(L, -3);
            lua_pushstring(L, "bid");
            pushNewClass(L, bidlist->at(i).bid);
        lua_settable(L, -3);
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
            fatal(error << "You tried to bid " << *b << " but " << bidlist->winner().player << " had already bid " << bidlist->winner().bid);
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

Hand::iterator Computer::findCard(Hand& collection, const SimpleCard* card) const {
    debug << *card;
    Hand::iterator it = std::find_if(collection.begin(), collection.end(), [=](const Card* c){ return *c == *card; });
    if(it != collection.end() && (*it)->value() == Card::JOKER)
        (*it)->setJokerSuit(card->suit(m_trumps));
    return it;
}

Hand Computer::yourTurnToSelectKitty(Hand& kitty) {
    debug;
    std::vector<Card*> discards(3);
    callLua(L, __func__, 1, [&](){
            pushList(L, 3, [&](int i){
                     pushClass<SimpleCard>(L, *kitty.at(i));
            });
    }, 1, [&](){
        if(lua_istable(L, lua_gettop(L))) {
            int tbl = lua_gettop(L);
            for(int i=0; i<3; ++i) {
                lua_pushnumber(L, i+1);
                lua_gettable(L, tbl);
                SimpleCard* c = lua_extract<SimpleCard>(L);
                debug << *c;
                Hand::iterator it;;
//                debug << **it;
                if((it = findCard(hand, c)) != hand.end()) {
                    discards[i] = *it;
                    hand.erase(it);
                } else if((it = findCard(kitty, c)) != kitty.end()) {
                    discards[i] = *it;
                    kitty.erase(it);
                } else {
                    fatal(error << "You tried to discard an invalid card or a card you already discarded: " << *c);
                }
            }
            for(Hand::const_iterator it = kitty.begin(); it != kitty.end(); ++it) {
                // anything left in the kitty is kept in the hand
                //if(findCard(discards, *it) == discards.end()) {
                    //(*it)->setFaceUp(m_play_open_hand);
                    emit turnUpCard(*it, m_play_open_hand);
                    hand.push_back(*it);
                //}
            }
            // update player hand
            registerHand();
            layoutHand();
            //emit this->kittyChosen(discards);

        } else
            fatal(error << __func__ << " must return table of discards");
        lua_pop(L, 1);
    });
    return discards;
}

void Computer::trickWon(const Trick& trick, const Player* winner) {
    debug;
    callLua(L, __func__, 2, [&](){
        pushList(L, trick.size(), [&](int i){
            lua_newtable(L);
            lua_pushstring(L, "player");
            lua_pushstring(L, trick.player(i)->name().toLocal8Bit());
            lua_settable(L, -3);
            lua_pushstring(L, "card");
            pushClass<SimpleCard>(L, *trick.card(i));
            lua_settable(L, -3);
        });
        lua_pushstring(L, winner->name().toLocal8Bit());
    });
    //emit acknowledgeTrick(this);
}

