#ifndef LOG_HPP
#define LOG_HPP

#include <sstream>
#include <vector>

class Log {
public:
    enum Level { TRACE, DEBUG, USER, INFO, ERROR };

    Log(Level level);
    virtual ~Log();

    std::stringstream& get(const char* funcname, const char* filename, int line);
    static Level ReportingLevel;
private:
    Level m_level;

    const char* m_funcname;
    const char* m_filename;
    int m_line;

    std::stringstream m_stream;
    friend std::ostream& operator<<(std::ostream& s, Log& log);

private:
    Log(const Log&);
    Log& operator=(const Log&);
};


class QString;
std::ostream& operator<<(std::ostream& s, const QString& str);
bool setLogLevel(const QString& str);

template<typename T>
std::ostream& operator<<(std::ostream& s, const std::vector<T*>& v) {
    for(typename std::vector<T*>::const_iterator it = v.begin(); it != v.end(); ++it)
        s << **it << (*it == v.back() ? "" : ", ");
    return s;
}

#define trace Log(Log::TRACE).get(__PRETTY_FUNCTION__, __FILE__, __LINE__)
#define debug Log(Log::DEBUG).get(__PRETTY_FUNCTION__, __FILE__, __LINE__)
#define info Log(Log::INFO).get(__PRETTY_FUNCTION__, __FILE__, __LINE__)
#define user Log(Log::USER).get(name, 0, 0)
#define error Log(Log::ERROR).get(__PRETTY_FUNCTION__, __FILE__, __LINE__)

void fatal(const std::basic_ostream<char>&) __attribute__((noreturn));


#endif // LOG_HPP
