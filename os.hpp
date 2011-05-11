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
#include <windows.h>
#define PURPLE  (FOREGROUND_BLUE | FOREGROUND_RED)
#define TEAL    (FOREGROUND_BLUE | FOREGROUND_GREEN)
#define YELLOW  (FORGEROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY)
#define GREEN   (FOREGROUND_GREEN)
#define RED     (FOREGROUND_RED | FOREGROUND_INTENSITY)
#define WHITE   (FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY)
#define NORMAL  0
#endif

class os
{
public:
    os();
    static char AI_PATH[512];
    static char GFX_PATH[512];
    static const char* getUserName();
#if defined(__linux)
    static void setStdoutColor(const char* colorcode);
#elif defined(_WIN32)
    static void setStdoutColor(unsigned colorcode);
#endif
};

#endif // OS_HPP
