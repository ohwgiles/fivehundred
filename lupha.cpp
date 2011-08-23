#include "lupha.hpp"

void callLua(lua_State* L, const char* funcname, int nargs, fnvoid argPusher, int nret, fnvoid returnHandler) {
    trace;
    //lua_getglobal(L, "Player");
    lua_getglobal(L, funcname);
    if(lua_isfunction(L, 1)) {
        //lua_getfield(L, -2, "_me");
        trace << lua_gettop(L);
        argPusher();
        trace << lua_gettop(L);
        if(lua_pcall(L, nargs, nret, 1) != 0)
            fatal(error << "Runtime error in " << funcname << ":\n" << luaL_checkstring(L, -1));
    } else
        fatal(error << "Could not find function " << funcname);
    returnHandler();
}
