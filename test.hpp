#ifndef TEST_HPP
#define TEST_HPP

#include "scriptable.hpp"

class Player;

class Test : public Scriptable {
public:
    Test(const QString& aifile, const QString& testfile);
    ~Test();
    void run();
    void setTrumpSuit(Suit trumps);
    void setLeadSuit(Suit lead);
};

#endif // TEST_HPP
