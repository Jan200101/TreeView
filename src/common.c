
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>

#include "common.h"

char buf[PATH_MAX];

struct stat getStat(const char* path)
{
    // fill with 0s by default in the case stat fails
    struct stat sb = {0};

    // the return value signifies if stat failes (e.g. file not found)
    // unimportant for us if it fails it won't touch sb
    stat(path, &sb);

    return sb;
}

inline char* resolveLink(const char* path)
{
    ssize_t len;
    if ((len = readlink(path, buf, sizeof(buf)-1)) != -1)
    {
        buf[len] = '\0';

        return buf;
    }

    return NULL;
}
int isLnk(const char* path)
{
    struct stat sb = getStat(path);

    return S_ISLNK(sb.st_mode);
}

int isFile(const char* path)
{
    // resolve symlink
    if (isLnk(path)) path = resolveLink(path);
    struct stat sb = getStat(path);

    return S_ISREG(sb.st_mode);
}

int isDir(const char* path)
{
    // resolve symlink
    if (isLnk(path)) path = resolveLink(path);
    struct stat sb = getStat(path);

    return S_ISDIR(sb.st_mode);
}

int isExe(const char* path)
{
    struct stat sb = getStat(path);

    return (sb.st_mode & S_IXUSR) != 0;
}

int makeDir(const char* path)
{
    char pathcpy[PATH_MAX];
    char *index;

    strncpy(pathcpy, path, PATH_MAX-1); // make a mutable copy of the path

    for(index = pathcpy+1; *index; ++index)
    {

        if (*index == '/')
        {
            *index = '\0';

            if (mkdir(pathcpy, 0755) != 0)
            {
                if (errno != EEXIST)
                    return -1;
            }

            *index = '/';
        }
    }

    return mkdir(path, 0755);
}

int strcicmp(const char *a, const char *b)
{
    for (;a && b; a++, b++) {
        int d = tolower(*a) - tolower(*b);
        if (d != 0 || !*a)
            return d;
    }
    return 0;
}