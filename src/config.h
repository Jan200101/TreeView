#ifndef CONFIG_H
#define CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

void getConfigDir(char* config, const size_t size);

size_t getLastPath(char* config, const size_t size);
size_t putLastPath(char* config, const size_t size);

#ifdef __cplusplus
}
#endif

#endif