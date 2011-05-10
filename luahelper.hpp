
#ifndef LUA_HELPER_H
#define LUA_HELPER_H

#include <functional>
#include <lua.hpp>
#include "log.hpp"

// Forward declare computer type
struct Computer;
struct lua_State;

typedef std::function<void()> fnvoid;
typedef std::function<void(int)> fnint;


template<typename T>
static inline T* lua_extract(lua_State* L) {
    trace;
    T **p = static_cast<T**>(luaL_checkudata(L, -1, T::className));
    if(!p) error << "Lua extraction failed, expecting " << T::className;
    lua_pop(L, 1);
    return *p;
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
    } else error << "At least one argument to equality operator must be of type " << T::className;
    return 1;
}

template<typename T>
static bool lua_cmp(lua_State*, const T&, const T&) { return false; }

template<class T>
static int lt(lua_State* L) {
    trace;
    lua_pushboolean(L, lua_cmp(L, *lua_extract<T>(L), *lua_extract<T>(L)));
    return 1;
}

template<class T>
static void register_additional_metas(lua_State*, int) {}



struct LuaTable {};

struct LuaRegistration {
    LuaRegistration(lua_State* L, Computer* computer) : L(L), computer(computer) {}

    inline void do_add_item(int num) {
        lua_pushnumber(L, num);
    }

    inline void do_add_item(LuaTable) {
        lua_newtable(L);
    }

    inline void do_add_item(const char* literal) {
        lua_pushstring(L, literal);
    }


    inline void do_add_item(void (*fn)(lua_State* L, Computer*)) {
        trace;
        lua_pushlightuserdata(L, computer);
        lua_pushlightuserdata(L, (void*)fn);
        lua_pushcclosure(L, [](lua_State* L) -> int {
            Computer* c = static_cast<Computer*>(lua_touserdata(L, lua_upvalueindex(1)));
            decltype(fn) f = (decltype(fn))(lua_touserdata(L, lua_upvalueindex(2)));
            (*f)(L, c);
            return 1;
        }, 2);
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
        lua_pushlightuserdata(L, computer);
        lua_pushcclosure(L, &lt<T>, 1);
        lua_settable(L, root+1);

        register_additional_metas<T>(L, root+1);

    /*
        lua_pushliteral(L, "__type");
        lua_pushstring(L, T::className);
        lua_settable(L, metatable);*/
        lua_setmetatable(L, root);

        add_item(root, args...);

        lua_pop(L, 1);
    }

    lua_State* L;
    Computer* computer;
};

void callLua(lua_State* L, const char* funcname, int nargs, fnvoid argPusher, int nret, fnvoid returnHandler) {
    trace;
    lua_getglobal(L, "Player");
    lua_getfield(L, lua_gettop(L), funcname);
    if(lua_isfunction(L, -1)) {
        lua_getfield(L, -2, "_me");
        argPusher();
        if(lua_pcall(L, nargs+1, nret, 0) != 0)
            error << "Runtime error in " << funcname << ": " << luaL_checklstring(L, -1, 0);
    } else
        error << "Could not find function " << funcname;
    returnHandler();
    lua_pop(L, 1);
}

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
        valuePusher(i);
        if(lua_gettop(L) != table + 2) error << "You screwed up the stack";
        lua_settable(L, table);
    }
}

template<typename T>
inline void pushClass(lua_State* L, T& cls) {
    trace;
    T** p = (T**)lua_newuserdata(L, sizeof(T*));
    *p = &cls;
    luaL_getmetatable(L, T::className);
    lua_setmetatable(L, -2);
}

template<typename T>
inline void pushNewClass(lua_State* L, T cls) {
    trace;
    T** p = (T**)lua_newuserdata(L, sizeof(T*));
    *p = new T(cls);
    trace << "making new " << T::className << ": " << cls << " at " << *p;
    luaL_getmetatable(L, T::className);
    lua_setmetatable(L, -2);
}

template<typename T>
inline void pushKeyValue(lua_State* L, const char* key, T valuePusher) {
    trace;
    int root = lua_gettop(L);
    lua_pushstring(L, key);
    valuePusher();
    if(lua_gettop(L) != root + 2) error << "You screwed up the stack";
    lua_settable(L, root);
}



#endif
