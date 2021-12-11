#ifndef COMMON_H
#define COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/stat.h>
#include <limits.h>

#ifndef PATH_MAX
#warning PATH_MAX not defined, setting it to 2048
#define PATH_MAX 2048
#endif

#ifdef PATH_SEP
#undef PATH_SEP
#endif

#ifdef PATH_SEP_STR
#undef PATH_SEP_STR
#endif

#ifdef _WIN32
	#define PATH_SEP '\\'
	#define PATH_SEP_STR "\\"
#else
	#define PATH_SEP '/'
	#define PATH_SEP_STR "/"
#endif

extern char buf[];

struct stat getStat(const char* path);
int isLnk(const char* path);
int isFile(const char* path);
int isDir(const char* path);
int isExe(const char* path);

int makeDir(const char* path);

int strcicmp(char const *a, char const *b);

#ifdef __cplusplus
}
#endif

#endif