#include <lua.hpp>
#include "suit.hpp"
#include <sstream>
#include <iostream>
#include "log.hpp"

typedef int TableIndex;

int lua_suit_complementary(lua_State* L) {
    Suit *s = static_cast<Suit*>(luaL_checkudata(L, 1, "Suit"));
    if(!s) luaL_typerror(L, 1, "Suit");
    lua_remove(L, 1);
    return 0;
}

template<class T>
int luabind_tostring(lua_State* L) {
	trace;
    T **s = static_cast<T**>(luaL_checkudata(L, 1, T::className));
    if(!s) luaL_typerror(L, 1, T::className);
    lua_remove(L, 1);
    std::stringstream ss;
    ss << **s;
    lua_pushstring(L, ss.str().c_str());
    return 1;
}

template<class T>
int luabind_equality(lua_State* L) {
	trace;
    T **op1 = static_cast<T**>(luaL_checkudata(L, 1, T::className));
    if(!op1) luaL_typerror(L, 1, T::className);
    lua_remove(L, 1);
    
    T **op2 = static_cast<T**>(luaL_checkudata(L, 1, T::className));
    if(!op2) luaL_typerror(L, 1, T::className);
    lua_remove(L, 1);

	lua_pushboolean(L, **op1 == **op2);
    return 1;
}

template<class T>
int luabind_lt(lua_State* L) {
    trace;
    T **op1 = static_cast<T**>(luaL_checkudata(L, 1, T::className));
    if(!op1) luaL_typerror(L, 1, T::className);
    lua_remove(L, 1);

    T **op2 = static_cast<T**>(luaL_checkudata(L, 1, T::className));
    if(!op2) luaL_typerror(L, 1, T::className);
    lua_remove(L, 1);

    lua_pushboolean(L, **op1 < **op2);
    return 1;
}

template<class T>
int luabind_index(lua_State* L) {
	trace;
    T **op1 = static_cast<T**>(luaL_checkudata(L, 1, T::className));
    if(!op1) luaL_typerror(L, 1, T::className);
    lua_remove(L, 1);
    
    T **op2 = static_cast<T**>(luaL_checkudata(L, 1, T::className));
    if(!op2) luaL_typerror(L, 1, T::className);
    lua_remove(L, 1);

	lua_pushboolean(L, **op1 == **op2);
    return 1;
}


template<class RetType, class Self, class Arg1>
RetType* luabind_callmethod(lua_State* L, const char* className, const char* funcname, Self* self, Arg1* arg1) {
    trace;
    lua_getglobal(L, className);
    lua_getfield(L, -1, funcname);
    if(lua_isfunction(L, -1)) {
        debug << "Found lua function " << funcname;

        Self** s = (Self**)lua_newuserdata(L, sizeof(Self*));
        *s = self;
        luaL_getmetatable(L, Self::className);
        lua_setmetatable(L, -2);

        Arg1** p1 = (Arg1**)lua_newuserdata(L, sizeof(Arg1*));
        *p1 = arg1;
        luaL_getmetatable(L, Arg1::className);
        lua_setmetatable(L, -2);

        lua_pcall(L, 2, 1, 0);
    } else
        error << "Could not find lua function " << funcname;
    lua_remove(L, 1);
    lua_remove(L, 1);
    lua_remove(L, 1);
    RetType** ret = static_cast<RetType**>(luaL_checkudata(L, 1, RetType::className));
    if(!ret) luaL_typerror(L, 1, RetType::className);
    lua_pop(L, 1);
    return *ret;
}

template<class T>
void luabind_registerinstance(lua_State* L, TableIndex tableloc, const char* name, T* instance) {
    lua_pushstring(L, name);
    T** p = (T**)lua_newuserdata(L, sizeof(T*));
    *p = instance;
    luaL_getmetatable(L, T::className);
    lua_setmetatable(L, -2);
    lua_settable(L, tableloc);
}

void luabind_registernumber(lua_State* L, TableIndex tableloc, const char* class_name, int value) {
    lua_pushstring(L, class_name);
    lua_pushnumber(L, value);
    lua_settable(L, tableloc);
}

void luabind_registermethod(lua_State* L, TableIndex tableloc, const char* method_name, int (fn)(lua_State*)) {
    lua_pushstring(L, method_name);
    lua_pushcfunction(L, fn);
    lua_settable(L, tableloc);
}

template<class T>
TableIndex luabind_registerclass(lua_State* L) {
	lua_newtable(L);
	int methods = lua_gettop(L);

	lua_pushstring(L, T::className);
	lua_pushvalue(L, methods);
	lua_settable(L, LUA_GLOBALSINDEX);

	luaL_newmetatable(L, T::className);
	int metatable = lua_gettop(L);

        /*lua_pushliteral(L, "__metatable");
        lua_pushliteral(L, "(hidden)");
        lua_settable(L, metatable);*/

        lua_newtable(L);

        lua_pushliteral(L, "__index");
        lua_pushvalue(L, -2);
        lua_settable(L, metatable);


                lua_pushliteral(L, "__newindex");
                lua_pushvalue(L, -2);
                lua_settable(L, metatable);
                lua_pop(L, 1);

	lua_pushliteral(L, "__tostring");
	lua_pushcfunction(L, luabind_tostring<T>);
	lua_settable(L, metatable);

	lua_pushliteral(L, "__eq");
	lua_pushcfunction(L, luabind_equality<T>);
	lua_settable(L, metatable);
/*
	lua_pushliteral(L, "__type");
	lua_pushstring(L, T::className);
        lua_settable(L, metatable);*/

	lua_setmetatable(L, methods);

	return methods;
}
