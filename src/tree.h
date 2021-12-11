#ifndef TREE_H
#define TREE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

struct FileTree {
    char* name;
    enum {
        FILE,
        DIRECTORY,

        UNKNOWN,
        NONE
    } type;

    struct FileTree* value;
    size_t count;
};

extern struct FileTree global_tree;

void populateTree(struct FileTree *tree, char* path, size_t size, int depth);
void freeTree(struct FileTree *tree);
void sortTree(struct FileTree *tree);


#ifdef __cplusplus
}
#endif

#endif