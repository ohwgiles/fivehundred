#include "os.hpp"

#include <cstdlib>
#include "log.hpp"

#if defined(__linux)
#include <sys/stat.h>
#elif defined(_WIN32)
#include <windows.h>
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
#elif defined(MACOSX)
#error "Todo"
#endif

}

const char* os::getUserName() {
#if defined(_WIN32)
    return getenv("USERNAME");
#else
    return getenv("USER");
#endif
}

static os _os;
