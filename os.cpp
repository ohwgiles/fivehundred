#include "os.hpp"

#include <cstdlib>
#include "log.hpp"

#if defined(__linux)
#include <sys/stat.h>
#include <unistd.h>
#elif defined(_WIN32)
#include <windows.h>
static CONSOLE_SCREEN_BUFFER_INFO csbi;
static HANDLE hstdout; = GetStdHandle( STD_OUTPUT_HANDLE );
#elif defined(MACOSX)
#error "Todo"
#else
#error "Unsupported platform"
#endif

char os::AI_PATH[];
char os::GFX_PATH[];

os::os()
{
#if defined(__linux)
	struct stat st;
	if(stat("/usr/share/fivehundred", &st) == 0) {
		sprintf(AI_PATH, "/usr/share/fivehundred/scripts/");
		sprintf(GFX_PATH, "/usr/share/fivehundred/gfx/");
	} else if(stat("/usr/local/share/fivehundred", &st) == 0) {
		sprintf(AI_PATH, "/usr/local/share/fivehundred/scripts/");
		sprintf(GFX_PATH, "/usr/local/share/fivehundred/gfx/");
	} else fatal(error<<"Could not find data directories");

#elif defined(_WIN32)
    SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, 0, AI_PATH);
    strcat(AI_PATH, "\\scripts\\");
    SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, 0, GFX_PATH);
    strcat(GFX_PATH, "\\gfx\\");
    hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(hstdout, &csbi);
#elif defined(MACOSX)
#error "Todo"
#endif

}

#if defined(__linux)
void os::setStdoutColor(const char* colorcode) {
    if(isatty(STDOUT_FILENO))
        fprintf(stdout, colorcode);
}
#elif defined(_WIN32)
void os::setStdoutColor(unsigned colorcode) {
    if(_isatty(hstdout)) {
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
