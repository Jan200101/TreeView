#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "common.h"
#include "config.h"

static void getXDGDir(const char* envvar, const char* homeext, char* config, const size_t size)
{
    char* xdg_var = getenv(envvar);

    if (xdg_var)
    {
        strncpy(config, xdg_var, size - 1);
        config[size-1] = '\0';
    }
    else
    {
        char* home = getenv("HOME");
        if (!home)
        {
#ifdef _WIN32
            home = getenv("USERPROFILE");
            if (!home)
#endif
            home = "";
        }
        if (!home) home = "";
        strncpy(config, home, size - 1);
        strncat(config, homeext, size - strlen(config) - 1);
        config[size-1] = '\0';
    }
}

void getConfigDir(char* config, const size_t size)
{
    getXDGDir("XDG_CONFIG_HOME", "/.config/" NAME, config, size);
}

size_t getLastPath(char* config, const size_t size)
{
	size_t retval = 0;
	char path[PATH_MAX];
	getConfigDir(path, sizeof(path));

	strncat(path, "/dir", sizeof(path) - strlen(path) - 1);

	if (isFile(path))
	{
		FILE* fd = fopen(path, "r");

		if (fd)
		{
			fseek(fd, 0L, SEEK_END);
			size_t fdsize = (size_t)ftell(fd);

			fseek(fd, 0L, SEEK_SET);

			if (fdsize > size) fdsize = size;

			retval = fread(config, sizeof(*config), fdsize, fd);
			fclose(fd);
		}
	}

	return retval;
}

size_t putLastPath(char* config, const size_t size)
{
	size_t retval = 0;
	char path[PATH_MAX];
	getConfigDir(path, sizeof(path));
    if (!isDir(path)) makeDir(path);

	strncat(path, "/dir", sizeof(path) - strlen(path) - 1);

	FILE* fd = fopen(path, "w");

	if (fd)
	{
		retval = fwrite(config, sizeof(*config), size, fd);
		fclose(fd);
	}

	return retval;
}