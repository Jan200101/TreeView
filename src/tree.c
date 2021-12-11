#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>

#include "common.h"
#include "main.h"
#include "tree.h"

 #include <libgen.h>

struct FileTree global_tree;

void populateTree(struct FileTree *tree, char* path, size_t size, int depth)
{
    if (done) return;

    if (!tree->name)
    {
        tree->name = (char*)malloc((size + 1) * sizeof(char));
        strncpy(tree->name, path, size);
    }
    tree->name[size] = '\0';

    tree->value = NULL;
    tree->count = 0;
    tree->type = UNKNOWN;

    if (isDir(path))
        tree->type = DIRECTORY;
    else if (isFile(path))
        tree->type = FILE;

    if (!depth) return;

    switch(tree->type)
    {
        case DIRECTORY:
            tree->value = (struct FileTree*)malloc(1);

            DIR *dir;
            struct dirent *ent;

            if ((dir = opendir(path)) != NULL)
            {
                while ((ent = readdir (dir)) != NULL)
                {
                    if (ent->d_name[0] == '.' && ( ent->d_name[1] == '\0' || ent->d_name[2] == '\0')) continue;

                    tree->count++;
                    tree->value = (struct FileTree*)realloc(tree->value, (tree->count * sizeof(struct FileTree)));

                    struct FileTree* index = &tree->value[tree->count-1];

                    size_t tmpbufsize = (strlen(path) + strlen(ent->d_name) + 2) * sizeof(char);
                    char* tmpbuf = (char*)malloc(tmpbufsize);
                    strncpy(tmpbuf, path, tmpbufsize);
                    strncat(tmpbuf, PATH_SEP_STR, tmpbufsize - strlen(tmpbuf) - 1);
                    strncat(tmpbuf, ent->d_name, tmpbufsize - strlen(tmpbuf) - 1);
                    tmpbuf[tmpbufsize-1] = '\0';

                    index->name = tmpbuf;
                }
                closedir(dir);
            }

            sortTree(tree);

            for (size_t i = 0; i < tree->count; ++i)
            {
                struct FileTree* index = &tree->value[i];
                populateTree(index, index->name, strlen(index->name), depth-1);
            }
            break;

        default:
            break;
    }
}

void freeTree(struct FileTree *tree)
{
    for (size_t i = 0; i < tree->count; ++i)
    {
        struct FileTree* child = &tree->value[i];
        freeTree(child);
    }

    if (tree->value) free(tree->value);
    tree->value = NULL;
    tree->count = 0;

    if (tree->name) free(tree->name);
    tree->name = NULL;
}

void sortTree(struct FileTree *tree)
{
    if (tree->type == DIRECTORY && tree->value)
    {
        int finished = 0;

        while (!finished)
        {
            finished = 1;

            for (size_t i = 1; i < tree->count; ++i)
            {
                struct FileTree temp;

                if (tree->value[i-1].name && tree->value[i-1].name && strcicmp(tree->value[i-1].name, tree->value[i].name) > 0)
                {
                    temp = tree->value[i-1];
                    tree->value[i-1] = tree->value[i];
                    tree->value[i] = temp;

                    finished = 0;
                }
            }
        }
    }
}
