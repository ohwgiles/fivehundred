#include "os.hpp"
/*!
  \file os.cpp
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
#include <cstdlib>
#include <iostream>

#if defined(__linux) 
    #include <sys/stat.h>
    #include <unistd.h>
    #include "log.hpp"
#elif defined(_WIN32)
    #include <io.h>
    #include <windows.h>
    #include <iostream>
    static CONSOLE_SCREEN_BUFFER_INFO csbi;
    static HANDLE hstdout;
#elif defined(__APPLE__)
    #include <unistd.h>
    #include <mach-o/dyld.h>
    #include "log.hpp"
#else
    #error "Unsupported platform"
#endif

QString os::AI_PATH;

QString os::GFX_PATH;

os::os()
{
#if defined(__linux)
    struct stat st;
    // Check whether the expected folder in /usr/share exists
    if(stat("/usr/share/fivehundred", &st) == 0) {
            AI_PATH = "/usr/share/fivehundred/scripts/";
            GFX_PATH = "/usr/share/fivehundred/gfx/";
    // otherwise fall back to /usr/local/share
    } else if(stat("/usr/local/share/fivehundred", &st) == 0) {
            AI_PATH = "/usr/local/share/fivehundred/scripts/";
            GFX_PATH = "/usr/local/share/fivehundred/gfx/";
    } else
        fatal(error<<"Could not find data directories");

#elif defined(_WIN32)
    wchar_t szAppPath[1024] = L"";
    wchar_t szAppDirectory[1024] = L"";
    // Get path to currently running executable
    GetModuleFileNameW(0, szAppPath, sizeof(szAppPath) - 1);
    // Get install path from that
    wcsncpy(szAppDirectory, szAppPath, wcsrchr(szAppPath, '\\') - szAppPath);
    // Hack to where the resources should be installed
    AI_PATH = QString::fromWCharArray(szAppDirectory) + "/scripts/";
    AI_PATH.replace("\\","/");
    GFX_PATH = QString::fromWCharArray(szAppDirectory) + "/gfx/";
    GFX_PATH.replace("\\","/");

    hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
    // Save the current console attributes
    GetConsoleScreenBufferInfo(hstdout, &csbi);
#elif defined(__APPLE__)
    char pathBuffer[1024];
    unsigned size = sizeof(pathBuffer);
    _NSGetExecutablePath(pathBuffer, &size);
    *strrchr(pathBuffer, '/') = 0;
    GFX_PATH = QString::fromLocal8Bit(pathBuffer) + "/../Resources/gfx/";
    AI_PATH = QString::fromLocal8Bit(pathBuffer) + "/../Resources/scripts/";
#endif

}

#if defined(__linux) || defined(__APPLE__)
void os::setStdoutColor(const char* colorcode) {
    // Only set the colours if stdout is a tty
    if(isatty(STDOUT_FILENO))
        fprintf(stdout, "%s", colorcode);
}
#elif defined(_WIN32)
void os::setStdoutColor(unsigned colorcode) {
    // Only set the colours if stdout is a tty
    if(_isatty(1)) {
        if(colorcode == 0)
            SetConsoleTextAttribute(hstdout, csbi.wAttributes);
        else
            SetConsoleTextAttribute(hstdout, colorcode);
    }
}
#endif

const char* os::getUserName() {
#if defined(_WIN32)
    return getenv("USERNAME");
#else
    return getenv("USER");
#endif
}

static os _os; //!< Static invocation of os object so that os::os gets called
