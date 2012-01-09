
#include "bid.hpp"
#include "player.hpp"
#include "card.hpp"
#include "trick.hpp"

#include "lupha.hpp"
#include "scriptable.hpp"

#define user


// Specialise comparison operators (from luapha.hpp)
template<>
bool lua_cmp<Bid>(lua_State*, const Bid& op1, const Bid& op2) { return op1 < op2; }

template<>
bool lua_cmp<SimpleCard>(lua_State* L, const SimpleCard& op1, const SimpleCard& op2) {
    Scriptable* c = static_cast<Scriptable*>(lua_touserdata(L, lua_upvalueindex(1)));
    Trick::Comparator comp = c->comparator();
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
            //Bid* b = new Bid(suit, numtricks);
            pushNewClass(L, Bid(suit, numtricks));
        } else {
            if(lua_isnumber(L, -1)) {
                pushNewClass(L, Bid(Bid::Type(lua_tonumber(L, -1))));
            } else {
                const char* type_str = luaL_checklstring(L, -1, 0);
                std::stringstream ss(type_str);
                Bid bid;
                ss >> bid;
                pushNewClass(L, bid);
            }
        }
        return 1;
    }, 0);

    lua_settable(L, tbl);
    lua_pushliteral(L, "__gc");

    lua_pushcclosure(L, [](lua_State* L) -> int {
        lua_delete<Bid>(L);
        return 0;
    }, 0);

    lua_settable(L, tbl);
}

template<>
void register_additional_metas<SimpleCard>(lua_State* L, int tbl) {
    trace;
    lua_pushliteral(L, "__call");
    lua_pushcclosure(L, [](lua_State* L) -> int {
        if(lua_gettop(L) == 3) {
            const char* suit_str = luaL_checklstring(L, 3, 0);
             if(lua_isnumber(L, 2)) {
                std::stringstream ss(suit_str);
                Suit suit;
                ss >> suit;
                if(ss.bad())
                    fatal(error << "Could not deserialise Suit: " << suit_str);
                 int value = luaL_checkinteger(L, 2);
                 lua_pop(L, 2);
                 pushNewClass(L,  SimpleCard(suit, Card::Value(value)));
                 return 1;
             } else
                 fatal(error<<"Card constructor expecting number for first arg");
        } else if(lua_gettop(L), 2) {
            if(lua_isnumber(L, 2)) {
                if(Card::Value(luaL_checkinteger(L, 2)) != Card::JOKER)
                    fatal(error<<"Unless meaning the joker, you must declare a suit when constructing a Card");
                lua_pop(L, 1);
                pushNewClass(L, SimpleCard(Suit::NONE, Card::JOKER));
                return 1;
            } else {
                const char* card_str = luaL_checklstring(L, 2, 0);
                std::stringstream ss(card_str);
                SimpleCard c;
                ss >> c;
                if(ss.bad())
                    fatal(error << "Could not deserialise Card: " << ss.str());
                lua_pop(L, 1);
                pushNewClass(L, c);
                return 1;
            }
        }
        return 0;
    }, 0);

    lua_settable(L, tbl);
    lua_pushliteral(L, "__gc");

    lua_pushcclosure(L, [](lua_State* L) -> int {
        lua_delete<SimpleCard>(L);
        return 0;
    }, 0);

    lua_settable(L, tbl);
}

static int computerLog(lua_State* L) {
    trace;
    std::string name = static_cast<Scriptable*>(lua_touserdata(L, lua_upvalueindex(1)))->name().toLocal8Bit().constData();
    std::stringstream output;
    while(lua_gettop(L) > 0) {
        if(lua_isstring(L, 1)) {
            output << lua_tostring(L, 1);
        } else if(lua_isnumber(L, 1)) {
            output << lua_tonumber(L, 1);
        } else if(lua_isboolean(L, 1)) {
            output << (lua_toboolean(L, 1) ? "True" : "False");
        } else if(lua_isuserdata(L, 1)) {
            lua_getmetatable(L, 1);
            lua_getfield(L, -1, "__tostring");
            lua_pushvalue(L, 1);
            lua_call(L, 1, 1);
            output << lua_tostring(L, -1);
            lua_pop(L, 2);
        } else {
            output << "<Unprintable type (" << luaL_typename(L, 1) << ")>";
        }
        lua_remove(L, 1);
        output << " ";
    }
    Log(Log::USER).get(name.c_str(), 0) << output.str();
    return 0;
}




Scriptable::Scriptable(const QString& script) :
    m_trumps(Suit::NONE),
    m_lead(Suit::NONE),
    L(0)
{
    L = lua_open();

    luaL_openlibs(L);

    lua_pushliteral(L, "print");
    lua_pushlightuserdata(L, (void*)this);
    lua_pushcclosure(L, &computerLog, 1);
    lua_settable(L, LUA_GLOBALSINDEX);

    lua_getglobal(L, "debug");
    lua_getfield(L, -1, "traceback");
    lua_remove(L, -2);

    Lupha<Scriptable> luareg(L, this);

    luareg.add_class<Bid>(
                "PASS", Bid::PASS,
                "NORMAL", Bid::NORMAL,
                "CLOSED_MISERE", Bid::CLOSED_MISERE,
                "OPEN_MISERE", Bid::OPEN_MISERE,
                "suit", [](lua_State* L, Scriptable*) {
                    std::ostringstream ss; ss << lua_extract<Bid>(L)->suit();
                    lua_pushstring(L, ss.str().c_str());
                },
                "type", [](lua_State* L, Scriptable*) {
                    lua_pushnumber(L, lua_extract<Bid>(L)->type());
                },
                "tricks", [](lua_State* L, Scriptable*) {
                    lua_pushnumber(L, lua_extract<Bid>(L)->tricks());
                }
              );
    luareg.add_class<SimpleCard>(
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
                "isTrump", [](lua_State* L, Scriptable* computer) {
                    lua_pushnumber(L, lua_extract<SimpleCard>(L)->suit(computer->m_trumps) == computer->m_trumps);
                },
                "suit", [](lua_State* L, Scriptable* computer) {
                    std::ostringstream ss;
                    ss << lua_extract<SimpleCard>(L)->suit(computer->m_trumps);
                    lua_pushstring(L, ss.str().c_str());
                },
                "value", [](lua_State* L, Scriptable* computer) {
                    lua_pushnumber(L, lua_extract<SimpleCard>(L)->value(computer->m_trumps));
                }
              );

    if(luaL_loadfile(L, script.toAscii()) != 0)
        fatal(error << lua_tostring(L, -1));

    if(lua_pcall(L, 0, 0, 1) != 0)
        fatal(error << "Error executing Lua script:\n" << lua_tostring(L, -1));

}

