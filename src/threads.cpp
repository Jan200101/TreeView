#include <cstring>

#include "threads.h"
#include "main.h"

std::thread tree_thread;

void spawnTreeThread(FileTree *tree, char* dir, size_t dirlen, int depth)
{
    // let us not interrupt a good working thread
    if (tree_thread.joinable())
        tree_thread.join();
    std::thread tmp_thread(populateTree, tree, dir, dirlen, depth);
    tree_thread.swap(tmp_thread);
}
