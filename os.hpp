#ifndef OS_HPP
#define OS_HPP
/*!
  \file os.hpp
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

// Color codes for printing log messages to a tty
#if defined(__linux)
#define PURPLE  "\033[0;35m"
#define TEAL    "\033[0;36m"
#define YELLOW  "\033[1;33m"
#define GREEN   "\033[0;32m"
#define RED     "\033[1;31m"
#define WHITE   "\033[1;37m"
#define NORMAL  "\033[m"
#elif defined(_WIN32)
#define PURPLE  (0x1 | 0x4)
#define TEAL    (0x1 | 0x2)
#define YELLOW  (0x2 | 0x4 | 0x8)
#define GREEN   (0x2)
#define RED     (0x4 | 0x8)
#define WHITE   (0x1 | 0x4 | 0x2 | 0x8)
#define NORMAL  0
#endif

#include <QString>

/*!
  \class os
  \brief Simple operating system abstraction layer

  This is a static class to abstract away differences in operating systems.

  It is a static class rather than a namespace so that we have the opportunity
  to perform some initialisation actions in its constructor
*/
struct os {
    os();

    static QString AI_PATH; //!< Filesystem path to AI scripts
    static QString GFX_PATH; //!< Filesystem path to graphics

    static const char* getUserName(); //!< Return the current user name (to use for player name)

    static void setStdoutColor(const char* colorcode); //!< Linux definition, will not be implemented otherwise
    static void setStdoutColor(unsigned colorcode); //!< Win32 definition, will not be implemented otherwise
};

#endif // OS_HPP
