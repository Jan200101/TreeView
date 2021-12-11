#ifndef THREADS_H
#define THREADS_H

#include <thread>

#include "tree.h"

extern std::thread tree_thread;;

void spawnTreeThread(FileTree *tree, char* dir, size_t dirlen, int depth);

#endif