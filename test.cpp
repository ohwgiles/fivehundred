#include "test.hpp"
/*!
  \file test.cpp
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

#include <fstream>

#include <iostream>
#include "os.hpp"
#include "bidding.hpp"
#include "deck.hpp"
#include "computer.hpp"
#include "trick.hpp"
#include "log.hpp"

#include "lua.hpp"

namespace {

int setTrumpSuit(lua_State* L) {
    Test* t = static_cast<Test*>(lua_touserdata(L, lua_upvalueindex(1)));
    std::stringstream ss(luaL_checkstring(L, 1));
    Suit s;
    ss >> s;
    t->setTrumpSuit(s);
    return 0;
}

int setLeadSuit(lua_State* L) {
    Test* t = static_cast<Test*>(lua_touserdata(L, lua_upvalueindex(1)));
    std::stringstream ss(luaL_checkstring(L, 1));
    Suit s;
    ss >> s;
    t->setLeadSuit(s);
    return 0;
}

std::string lua_asstring(lua_State* L, int index) {
    if(lua_isuserdata(L, index)) {
        if(index < 0) index = lua_gettop(L) + index + 1;
        lua_getmetatable(L, index);
        lua_getfield(L, -1, "__tostring");
        lua_remove(L, -2);
        lua_pushvalue(L, index);
        lua_call(L, 1, 1);
        std::string result = luaL_checkstring(L, -1);
        lua_pop(L, 1);
        return result;
    } else
        return lua_tostring(L, index);
}

int expect_eq(lua_State* L) {
    if(!lua_equal(L, 1, 2)) {
        luaL_where(L, 1);
        error << lua_tostring(L, -1) << "Expectation " << lua_asstring(L, 1) << " == " << lua_asstring(L, 2) << " failed!";
        lua_pop(L, 1);
    }
    return 0;
}

int expect_ne(lua_State* L) {
    if(lua_equal(L, 1, 2)) {
        luaL_where(L, 1);
        error << lua_tostring(L, -1) << "Expectation " << lua_asstring(L, 1) << " != " << lua_asstring(L, 2) << " failed!";
        lua_pop(L, 1);
    }
    return 0;
}

int expect_lt(lua_State* L) {
    if(!lua_lessthan(L, 1, 2)) {
        luaL_where(L, 1);
        error<< lua_tostring(L, -1)  << "Expectation " << lua_asstring(L, 1) << " < " << lua_asstring(L, 2) << " failed!";
        lua_pop(L, 1);
    }
    return 0;
}

int expect_gt(lua_State* L) {
    if(!lua_lessthan(L, 2, 1)) {
        luaL_where(L, 1);
        error<< lua_tostring(L, -1)  << "Expectation " << lua_asstring(L, 1) << " > " << lua_asstring(L, 2) << " failed!";
        lua_pop(L, 1);
    }
    return 0;
}

int expect_true(lua_State* L) {
    if(!lua_toboolean(L, 1)) {
        luaL_where(L, 1);
        error<< lua_tostring(L, -1)  << "Expectation 'true' failed!";
        lua_pop(L, 1);
    }
    return 0;
}

int expect_false(lua_State* L) {
    if(lua_toboolean(L, 1)) {
        luaL_where(L, 1);
        error<< lua_tostring(L, -1)  << "Expectation 'false' failed!";
        lua_pop(L, 1);
    }
    return 0;
}



}

void Test::setTrumpSuit(Suit trumps) { m_trumps = trumps; }
void Test::setLeadSuit(Suit lead) { m_lead = lead; }

Test::Test(const QString& aifile, const QString& testfile) :
    Scriptable(aifile)
{
    lua_pushstring(L, "ME");
    lua_pushnumber(L, 1);
    lua_settable(L, LUA_GLOBALSINDEX);
    lua_pushstring(L, "LEFT");
    lua_pushnumber(L, 2);
    lua_settable(L, LUA_GLOBALSINDEX);
    lua_pushstring(L, "PARTNER");
    lua_pushnumber(L, 3);
    lua_settable(L, LUA_GLOBALSINDEX);
    lua_pushstring(L, "RIGHT");
    lua_pushnumber(L, 4);
    lua_settable(L, LUA_GLOBALSINDEX);

    lua_pushstring(L, "setTrumps");
    lua_pushlightuserdata(L, this);
    lua_pushcclosure(L, &::setTrumpSuit, 1);
    lua_settable(L, LUA_GLOBALSINDEX);

    lua_pushstring(L, "setLeadSuit");
    lua_pushlightuserdata(L, this);
    lua_pushcclosure(L, &::setLeadSuit, 1);
    lua_settable(L, LUA_GLOBALSINDEX);

    lua_pushstring(L, "expect_eq");
    lua_pushcfunction(L, &expect_eq);
    lua_settable(L, LUA_GLOBALSINDEX);
    lua_pushstring(L, "expect_ne");
    lua_pushcfunction(L, &expect_ne);
    lua_settable(L, LUA_GLOBALSINDEX);
    lua_pushstring(L, "expect_lt");
    lua_pushcfunction(L, &expect_lt);
    lua_settable(L, LUA_GLOBALSINDEX);
    lua_pushstring(L, "expect_gt");
    lua_pushcfunction(L, &expect_gt);
    lua_settable(L, LUA_GLOBALSINDEX);
    lua_pushstring(L, "expect_true");
    lua_pushcfunction(L, &expect_true);
    lua_settable(L, LUA_GLOBALSINDEX);
    lua_pushstring(L, "expect_false");
    lua_pushcfunction(L, &expect_false);
    lua_settable(L, LUA_GLOBALSINDEX);


    if(luaL_loadfile(L, testfile.toAscii()) != 0)
        fatal(error << lua_tostring(L, -1));

    if(lua_pcall(L, 0, 0, 1) != 0)
        fatal(error << "Error executing Lua script:\n" << lua_tostring(L, -1));
}

