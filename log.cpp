#include "os.hpp"
#include "log.hpp"
#include <cstdio>

#include <QString>

Log::Level Log::ReportingLevel = Log::USER;


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
            printf("[");
            os::setStdoutColor(YELLOW);
            printf("%s", str_level);
            os::setStdoutColor(NORMAL);
            printf("] (%s): ", m_funcname);
            os::setStdoutColor(WHITE);
            printf("%s", m_stream.str().c_str());
            os::setStdoutColor(NORMAL);
            printf("\n");
            break;
        default:
            if(m_stream.str().empty()) {
                printf("[");
                os::setStdoutColor(COLOR(m_level));
                printf("%s", str_level);
                os::setStdoutColor(NORMAL);
                printf("] - %s:%d - %s\n", m_filename, m_line, m_funcname);
            } else {
                printf("[");
                os::setStdoutColor(COLOR(m_level));
                printf("%s", str_level);
                os::setStdoutColor(NORMAL);
                printf("] - %s:%d - %s : ", m_filename, m_line, m_funcname);
                os::setStdoutColor(WHITE);
                printf("%s", m_stream.str().c_str());
                os::setStdoutColor(NORMAL);
                printf("\n");
            }
            break;
        }
        fflush(stdout);
    }
}

void fatal(const std::basic_ostream<char>& ss) {
    throw fivehundrederror(static_cast<const std::stringstream&>(ss).str().c_str());
}
