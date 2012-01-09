#ifndef TEST_HPP
#define TEST_HPP
/*!
  \file test.hpp
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

#include "scriptable.hpp"

class Player;

/*!
  \class Test
  \brief Facilitates testing player AIs by providing a lua test framework
*/
class Test : public Scriptable {
public:
    Test(const QString& aifile, const QString& testfile);
    virtual ~Test() {}
    void run();
    // Callable from your lua test
    void setTrumpSuit(Suit trumps);
    void setLeadSuit(Suit lead);
};

#endif // TEST_HPP
