#include "log.hpp"
#include <cstdio>

#include <QString>

Log::Level Log::ReportingLevel = Log::USER;

static char log_str[][16] = {
    "\033[0;35mTRACE\033[m",
    "\033[0;36mDEBUG\033[m",
    "\033[1;33mUSER \033[m",
    "\033[0;32mINFO \033[m",
    "\033[1;31mERROR\033[m"
};

Log::Log(Level level) :
        m_level(level)
{

}

std::stringstream& Log::get(const char* funcname, const char* filename, int line) {
    m_funcname = funcname;
    m_filename = filename;
    m_line = line;
    return m_stream;
}

bool setLogLevel(const QString& str) {
    if(str == "TRACE") Log::ReportingLevel = Log::TRACE;
    else if(str == "DEBUG") Log::ReportingLevel = Log::DEBUG;
    else if(str == "USER") Log::ReportingLevel = Log::USER;
    else if(str == "INFO") Log::ReportingLevel = Log::INFO;
    else if(str == "ERROR") Log::ReportingLevel = Log::ERROR;
    else return false;
    return true;
}

std::ostream& operator<<(std::ostream& s, Log& log) {
    log.m_stream << s;
    return s;
}

std::ostream& operator<<(std::ostream& s, const QString& str) {
    s << str.toAscii().constData();
    return s;
}

struct fivehundrederror : public std::exception {
    fivehundrederror(const char* str) : str(str) {}
    const char* str;
    virtual const char* what() const throw() { return str; }
};

Log::~Log() {

    if(m_level >= ReportingLevel) {
        char* str_level = log_str[int(m_level)];
        switch(m_level) {
        case USER:
            fprintf(stderr, "[%s] (%s): \033[1;37m%s\033[m\n", str_level, m_funcname, m_stream.str().c_str());
            break;
        default:
            if(!m_stream.str().empty())
                fprintf(stderr, "[%s] - %s:%d - %s : \033[1;37m%s\033[m\n", str_level, m_filename, m_line, m_funcname, m_stream.str().c_str());
            else
                fprintf(stderr, "[%s] - %s:%d - %s\n", str_level, m_filename, m_line, m_funcname);

            break;
        }
        fflush(stderr);
    }
    //if(m_level == Log::ERROR)
    //    throw fivehundrederror(m_stream.str().c_str());

}

void fatal(const std::basic_ostream<char>& ss) {
    throw fivehundrederror(static_cast<const std::stringstream&>(ss).str().c_str());
}
