#ifndef LOG_HPP
#define LOG_HPP
/*!
  \file log.hpp
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
#include <sstream>
#include <vector>

class QString;
/*!
  \class Log
  \brief Basic logging functionality

  Provides 5 main logging levels with one (USER) reserved for
  output from Lua scripts or other user function. Logging will be
  in color if stdout is connected to a tty.
*/
class Log {
public:
    enum Level { TRACE, DEBUG, USER, INFO, ERROR };

    Log(Level level);
    virtual ~Log();

    std::stringstream& get(const char* funcname, int line);

    static bool setLogLevel(const QString& str); //!< Set the global log level from a string (e.g. from commandline argument)
    static Level ReportingLevel; //!< The global log level

private:
    Level m_level;

    const char* m_funcname;
    int m_line;

    std::stringstream m_stream;

private:
    Log(const Log&);
    Log& operator=(const Log&);
};


//! This is here cos I couldn't think of a better place. It's a convenience to log QStrings as ASCII
std::ostream& operator<<(std::ostream& s, const QString& str);


//! Convenience function to print a vector to any ostream
template<typename T>
std::ostream& operator<<(std::ostream& s, const std::vector<T*>& v) {
    for(typename std::vector<T*>::const_iterator it = v.begin(); it != v.end(); ++it)
        s << **it << (*it == v.back() ? "" : ", ");
    return s;
}

#define trace Log(Log::TRACE).get(__PRETTY_FUNCTION__, __LINE__)
#define debug Log(Log::DEBUG).get(__PRETTY_FUNCTION__, __LINE__)
#define info Log(Log::INFO).get(__PRETTY_FUNCTION__, __LINE__)
#define error Log(Log::ERROR).get(__PRETTY_FUNCTION__, __LINE__)

//! Converts a Log object into an exception and throws it
void fatal(const std::basic_ostream<char>&) __attribute__((noreturn));


#endif // LOG_HPP
