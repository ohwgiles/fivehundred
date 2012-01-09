#include "lupha.hpp"
/*!
  \file lupha.cpp
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

void callLua(lua_State* L, const char* funcname, int nargs, fnvoid argPusher, int nret, fnvoid returnHandler) {
    trace;
    lua_getglobal(L, funcname);
    if(lua_isfunction(L, 1)) {
        trace << lua_gettop(L);
        argPusher();
        trace << lua_gettop(L);
        if(lua_pcall(L, nargs, nret, 1) != 0)
            fatal(error << "Runtime error in " << funcname << ":\n" << luaL_checkstring(L, -1));
    } else
        fatal(error << "Could not find function " << funcname);
    returnHandler();
}
