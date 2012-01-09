#ifndef LUPHA_HPP
#define LUPHA_HPP
/*!
  \file lupha.hpp
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
#include <functional>
#include <lua.hpp>
#include "log.hpp"

/*!
  \file lupha.hpp
  This file is a convenience c++ layer for the lua api
*/

// Forward declare computer type
struct lua_State;

typedef std::function<void()> fnvoid;
typedef std::function<void(int)> fnint;

template<typename T>
struct LuaMemObj {
    bool deleteMe;
    T* ptr;
};

template<typename T>
static inline T* lua_extract(lua_State* L) {
    trace;
    LuaMemObj<T>* p = static_cast<LuaMemObj<T>*>(luaL_checkudata(L, -1, T::className));
    if(!p)
        fatal(error << "Lua extraction failed, expecting " << T::className);
    lua_pop(L, 1);
    return p->ptr;
}

template<typename T>
static inline void lua_delete(lua_State* L) {
    trace;
    LuaMemObj<T>* p = static_cast<LuaMemObj<T>*>(luaL_checkudata(L, -1, T::className));
    if(!p)
        fatal(error << "Lua extraction failed, expecting " << T::className);
    lua_pop(L, 1);
    trace << "asked to delete " << *p->ptr << " at " << p->ptr << ", deleting: " << p->deleteMe;
    if(p->deleteMe)
        delete p->ptr;
}

template<class T>
static int tostring(lua_State* L) {
    trace;
    std::stringstream ss;
    ss << *lua_extract<T>(L);
    lua_pushstring(L, ss.str().c_str());
    return 1;
}

template<class T>
static int equality(lua_State* L) {
    trace;
    bool op1_udata = bool(luaL_checkudata(L, -1, T::className));
    bool op2_udata = bool(luaL_checkudata(L, -2, T::className));
    if(op1_udata && op2_udata) {
        lua_pushboolean(L, *lua_extract<T>(L) == *lua_extract<T>(L));
    } else if(op1_udata || op2_udata) {
        if(luaL_checkudata(L, -1, T::className)) {
            T* op1 = lua_extract<T>(L);
            T op2; std::stringstream str(luaL_checklstring(L, -1, 0));
            str >> op2;
            lua_pop(L, 1);
            lua_pushboolean(L, *op1 == op2);
        } else {
            T op2; std::stringstream str(luaL_checklstring(L, -1, 0));
            str >> op2;
            lua_pop(L, 1);
            T* op1 = lua_extract<T>(L);
            lua_pushboolean(L, *op1 == op2);
        }
    } else
        fatal(error << "At least one argument to equality operator must be of type " << T::className);
    return 1;
}

template<typename T>
static bool lua_cmp(lua_State*, const T&, const T&) {
    error << "No specialised template for type " << T::classname;
    return false;
}

template<class T>
static int lt(lua_State* L) {
    trace;
    lua_pushboolean(L, lua_cmp(L, *lua_extract<T>(L), *lua_extract<T>(L)));
    return 1;
}

template<class T>
static void register_additional_metas(lua_State*, int) {}

struct LuaTable {};

template<class User>
struct Lupha {
    Lupha(lua_State* L, User* user) : L(L), user(user) {}

    inline void do_add_item(int num) {
        lua_pushnumber(L, num);
    }

    inline void do_add_item(LuaTable) {
        lua_newtable(L);
    }

    inline void do_add_item(const char* literal) {
        lua_pushstring(L, literal);
    }

    static int call_back_user(lua_State* L) {
        User* u = static_cast<User*>(lua_touserdata(L, lua_upvalueindex(1)));
        void (*f)(lua_State*, User*) = (void (*)(lua_State*, User*)) (lua_touserdata(L, lua_upvalueindex(2)));
        (*f)(L, u);
        return 1;
    }

    inline void do_add_item(void (*fn)(lua_State* L, User* user)) {
        trace;
        lua_pushlightuserdata(L, user);
        lua_pushlightuserdata(L, (void*)fn);
        lua_pushcclosure(L, &call_back_user, 2);
    }

    inline void add_item(int) const {}

    template<typename T, typename... U>
    inline void add_item(int stackindex, const char* name, const T& item, const U&... rem) {
        trace;
        lua_pushstring(L, name);
        do_add_item(item);
        lua_settable(L, stackindex);
        add_item(stackindex, rem...);
    }

    template<typename T, typename... U>
    void add_class(U... args) {
        trace;

        lua_newtable(L);
        int root = lua_gettop(L);

        lua_pushstring(L, T::className);
        lua_pushvalue(L, root);
        lua_settable(L, LUA_GLOBALSINDEX);

        luaL_newmetatable(L, T::className);
        lua_pushliteral(L, "__metatable");
        lua_pushliteral(L, "(hidden)");
        lua_settable(L, root+1);

        lua_newtable(L);
        lua_pushliteral(L, "__index");
        lua_pushvalue(L, -2);
        lua_settable(L, root+1);
        lua_pushliteral(L, "__newindex");
        lua_pushvalue(L, -2);
        lua_settable(L, root+1);
        lua_pop(L, 1);

        lua_pushliteral(L, "__tostring");
        lua_pushcfunction(L, &tostring<T>);
        lua_settable(L, root+1);

        lua_pushliteral(L, "__eq");
        lua_pushcclosure(L, &equality<T> ,0);
        lua_settable(L, root+1);

        lua_pushliteral(L, "__lt");
        lua_pushlightuserdata(L, user);
        lua_pushcclosure(L, &lt<T>, 1);
        lua_settable(L, root+1);

        register_additional_metas<T>(L, root+1);

        lua_setmetatable(L, root);

        add_item(root, args...);

        lua_pop(L, 1);
    }

    lua_State* L;
    User* user;
};

void callLua(lua_State* L, const char* funcname, int nargs, fnvoid argPusher, int nret, fnvoid returnHandler);

inline void callLua(lua_State* L, const char* funcname, int nargs, fnvoid argPusher) {
    trace;
    callLua(L, funcname, nargs, argPusher, 0, [](){});
}

inline void callLua(lua_State* L, const char* funcname) {
    trace;
    callLua(L, funcname, 0, [](){}, 0, [](){});
}

inline void pushList(lua_State* L, int numItems, fnint valuePusher) {
    trace;
    lua_newtable(L);
    int table = lua_gettop(L);
    for(int i = 0; i < numItems; ++i) {
        lua_pushnumber(L, i+1);
        trace;
        valuePusher(i);
        trace;
        lua_settable(L, table);
        trace;
    }
}

template<typename T>
inline void pushClass(lua_State* L, T& cls) {
    trace;
    LuaMemObj<T>* p = (LuaMemObj<T>*)lua_newuserdata(L, sizeof(LuaMemObj<T>));
    p->deleteMe = false;
    p->ptr = &cls;
    trace;
    luaL_getmetatable(L, T::className);
    trace;
    lua_setmetatable(L, -2);
    trace;
}


template<typename T>
inline void pushNewClass(lua_State* L, T cls) {
    trace;
    LuaMemObj<T>* p = (LuaMemObj<T>*)lua_newuserdata(L, sizeof(LuaMemObj<T>));
    p->deleteMe = true;
    p->ptr = new T(cls);
    trace << "making new " << T::className << ": " << cls << " at " << p->ptr;
    luaL_getmetatable(L, T::className);
    lua_setmetatable(L, -2);
}



#endif
