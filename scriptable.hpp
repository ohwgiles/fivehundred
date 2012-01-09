#ifndef SCRIPTABLE_HPP
#define SCRIPTABLE_HPP
/*!
  \file scriptable.hpp
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

#include <QString>

#include "suit.hpp"
#include "trick.hpp"


struct lua_State;

/*!
  \class Scriptable
  \brief Represents a Lua-scriptable object

  Lua-scriptable objects come in two flavours, computer AIs and tests
  for them. This provides a common base class for shared functionality.
*/
class Scriptable
{
public:
    Scriptable(const QString& script);
    // this is a bit of tricky indirection. For a regular scriptable object, always assume
    // the joker is a winner (noJokerSuitHigh = true). In real play, the joker may be
    // played offsuit, so this function must be overridden
    virtual Trick::Comparator comparator() { return Trick::Comparator(m_trumps, m_lead); }
    virtual QString name() { return "ScriptUnderTest"; }
protected:
    Suit m_trumps;
    Suit m_lead;
    lua_State* L;

};

#endif // SCRIPTABLE_HPP
