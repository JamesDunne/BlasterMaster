#ifndef WIN32
#include <unistd.h>
#else
#include <windows.h>
#include <direct.h>
#endif

#include <signal.h>
#include <stdlib.h>
#include <limits.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#ifndef WIN32
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <mntent.h>
#include <dlfcn.h>
#endif

#include "bm_game.h"

void UnloadGame (void **game_library) {
#ifdef WIN32
	if (!FreeLibrary (*game_library))
		fprintf(stderr, "FreeLibrary failed!\n");
#else
	if (*game_library)
		dlclose (*game_library);
#endif
	*game_library = NULL;
}

// Load the game dll's functions:
dllfunctions *GetGameAPI (const char *tempname, hostfunctions *thost, void **game_library) {
	dllfunctions *(*getfuncs) (hostfunctions *);

	char	curpath[8192];
	char	name[8192];
	char	gamename[8192];

	// Append the extension to the file:
	strcpy(gamename, tempname);
#ifdef WIN32
	strcat(gamename, ".dll");
#else
	strcat(gamename, ".so");
#endif

#ifndef WIN32
	setreuid(getuid(), getuid());
	setegid(getgid());
#endif

	if (*game_library) {
		fprintf(stderr, "Game library already loaded!\n");
		return NULL;
	}

	// Grab the working directory
	getcwd(curpath, 8192);
	// Prepend it to the name of the dll:
	sprintf(name, "%s/%s", curpath, gamename);

#ifdef WIN32
	*game_library = LoadLibrary (name);
#else
	*game_library = dlopen (name, RTLD_LAZY );
#endif
	if (!(*game_library)) {
		fprintf(stderr, "dlopen (%s) failed!\n", name);
		return NULL;
	}

#ifdef WIN32
	getfuncs = ( dllfunctions *(*)(hostfunctions *) )GetProcAddress (*game_library, "DLLInitialize");
#else
	getfuncs = ( dllfunctions *(*)(hostfunctions *) )dlsym (*game_library, "DLLInitialize");
#endif
	if (!getfuncs) {
		fprintf(stderr, "DLLInitialize() was not exported by the DLL!\n");
		UnloadGame(*game_library);
		return NULL;
	};

	return getfuncs (thost);
}

