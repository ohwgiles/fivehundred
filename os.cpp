#include "os.hpp"

#include <cstdlib>

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
#elif defined(MACOSX)
#error "Todo"
#else
#error "Unsupported platform"
#endif

QString os::AI_PATH;
QString os::GFX_PATH;

os::os()
{
#if defined(__linux)
	struct stat st;
	if(stat("/usr/share/fivehundred", &st) == 0) {
		AI_PATH = "/usr/share/fivehundred/scripts/";
		GFX_PATH = "/usr/share/fivehundred/gfx/";
	} else if(stat("/usr/local/share/fivehundred", &st) == 0) {
		AI_PATH = "/usr/local/share/fivehundred/scripts/";
		GFX_PATH = "/usr/local/share/fivehundred/gfx/";
	} else fatal(error<<"Could not find data directories");
#elif defined(_WIN32)
	wchar_t szAppPath[1024] = L"";
	wchar_t szAppDirectory[1024] = L"";
	GetModuleFileNameW(0, szAppPath, sizeof(szAppPath) - 1);
	wcsncpy(szAppDirectory, szAppPath, wcsrchr(szAppPath, '\\') - szAppPath);
	std::wcout << szAppPath << std::endl;
	AI_PATH = QString::fromWCharArray(szAppDirectory) + "/../share/fivehundred/scripts/";
	AI_PATH.replace("\\","/");
	GFX_PATH = QString::fromWCharArray(szAppDirectory) + "/../share/fivehundred/gfx/";
	GFX_PATH.replace("\\","/");
	std::cout << AI_PATH.toAscii().constData() << std::endl;
    hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(hstdout, &csbi);
#elif defined(MACOSX)
#error "Todo"
#endif

}

#if defined(__linux)
void os::setStdoutColor(const char* colorcode) {
    if(isatty(STDOUT_FILENO))
        fprintf(stdout, "%s", colorcode);
}
#elif defined(_WIN32)
void os::setStdoutColor(unsigned colorcode) {
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

static os _os;
