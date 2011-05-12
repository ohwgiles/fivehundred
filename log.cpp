#include "log.hpp"
/*!
  \file log.cpp
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
#include "os.hpp"
#include <cstdio>
#include <QString>

//! USER is the default reporting level
Log::Level Log::ReportingLevel = Log::USER;

//! The strings to display for each log level
static char log_str[][6] = {
    "TRACE",
    "DEBUG",
    "USER ",
    "INFO ",
    "ERROR"
};

#define COLOR(loglevel) ( \
    loglevel==Log::TRACE ? PURPLE :\
    loglevel==Log::DEBUG ? TEAL :\
    loglevel==Log::USER  ? YELLOW :\
    loglevel==Log::INFO  ? GREEN :\
    loglevel==Log::ERROR ? RED : 0)

Log::Log(Level level) :
        m_level(level)
{

}

std::stringstream& Log::get(const char* funcname, int line) {
    m_funcname = funcname;
    m_line = line;
    return m_stream;
}

bool Log::setLogLevel(const QString& str) {
    if(str == "TRACE") Log::ReportingLevel = Log::TRACE;
    else if(str == "DEBUG") Log::ReportingLevel = Log::DEBUG;
    else if(str == "USER") Log::ReportingLevel = Log::USER;
    else if(str == "INFO") Log::ReportingLevel = Log::INFO;
    else if(str == "ERROR") Log::ReportingLevel = Log::ERROR;
    else return false;
    return true;
}


std::ostream& operator<<(std::ostream& s, const QString& str) {
    s << str.toAscii().constData();
    return s;
}

Log::~Log() {
    if(m_level >= ReportingLevel) {
        char* str_level = log_str[int(m_level)];
        switch(m_level) {
        case USER:
            printf("[");
            os::setStdoutColor(YELLOW);
            printf("%s", str_level);
            os::setStdoutColor(NORMAL);
            printf("] %s: ", m_funcname);
            os::setStdoutColor(WHITE);
            printf("%s", m_stream.str().c_str());
            os::setStdoutColor(NORMAL);
            printf("\n");
            break;
        default:
            printf("[");
            os::setStdoutColor(COLOR(m_level));
            printf("%s", str_level);
            os::setStdoutColor(NORMAL);
            printf("] %s:%d", m_funcname, m_line);
            if(!m_stream.str().empty()) {
                printf(": ");
                os::setStdoutColor(WHITE);
                printf("%s", m_stream.str().c_str());
                os::setStdoutColor(NORMAL);
            }
            printf("\n");
            break;
        }
        fflush(stdout);
    }
}

/*!
  \class FiveHundredException
  \brief Basic exception class to abort the app when shit hits the fan

  This class is also useful in tests where you can test that objects
  fail where they should, rather than just calling exit(1)
*/
struct FiveHundredException : public std::exception {
    FiveHundredException(const char* str) : str(str) {}
    const char* str;
    virtual const char* what() const throw() { return str; }
};

void fatal(const std::basic_ostream<char>& ss) {
    throw FiveHundredException(static_cast<const std::stringstream&>(ss).str().c_str());
}
