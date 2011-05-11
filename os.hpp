#ifndef OS_HPP
#define OS_HPP

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

class os
{
public:
    os();
    static QString AI_PATH;
    static QString GFX_PATH;
    static const char* getUserName();
#if defined(__linux)
    static void setStdoutColor(const char* colorcode);
#elif defined(_WIN32)
    static void setStdoutColor(unsigned colorcode);
#endif
};

#endif // OS_HPP
