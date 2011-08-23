#ifndef SCRIPTABLE_HPP
#define SCRIPTABLE_HPP

#include <QString>

#include "suit.hpp"
#include "trick.hpp"


struct lua_State;

class Scriptable
{
public:
    Scriptable(const QString& script);
    Trick::Comparator comparator() { return {m_trumps, m_lead}; }
    virtual QString name() { return "ScriptUnderTest"; }
protected:
    Suit m_trumps;
    Suit m_lead;
    lua_State* L;

};

#endif // SCRIPTABLE_HPP
