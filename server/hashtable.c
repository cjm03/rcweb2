#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <math.h>
#include <dirent.h>
#include <sys/stat.h>

#include "hashtable.h"

int hash(const char* id)
{
    long hash = 0;
    int len = strlen(id);
    for (int i = 0; i < len; i++) {
        hash += (long)pow(151, len - (i + 1)) * id[i];
        hash = hash % TABLE_SIZE;
    }
    return (int)hash;
}

Table* createTable(void)
{
    Table* t = malloc(sizeof(Table));
    if (t != NULL) {
        memset(t->items, 0, sizeof(t->items));
    }
    return t;
}

void insertItem(Table* t, const char* id, const char* path, size_t size)
{
    int index = hash(id);
    Item* new = malloc(sizeof(Item));
    if (new != NULL) {
        new->id = strdup(id);
        new->path = strdup(path);
        new->size = size;
        new->next = t->items[index];
        t->items[index] = new;
    }
}

Item* getItem(Table* t, const char* id)
{
    int index = hash(id);
    Item* cur = t->items[index];
    while (cur != NULL) {
        if (strcmp(cur->id, id) == 0) {
            return cur;
        }
        cur = cur->next;
    }
    return NULL;
}

void freeTable(Table* t)
{
    if (!t) return;

    for (int i = 0; i < TABLE_SIZE; ++i) {
        Item* cur = t->items[i];
        while (cur != NULL) {
            Item* next = cur->next;
            free(cur->id);
            free(cur->path);
            free(cur);
            cur = next;
        }
    }
    free(t);
}

//=================================
// loadClipsFromDir - Helper func
//=================================

void loadClipsFromDir(Table* t, const char* directory)
{
    DIR* dir = opendir(directory);
    if (!dir) {
        perror("opendir");
        return;
    }
    struct dirent* entry;
    while ((entry = readdir(dir))) {
        if (entry->d_type == DT_REG) {
            if (strstr(entry->d_name, ".mp4")) {
                char id[256] = {0};
                char filepath[512] = {0};

                snprintf(id, sizeof(id), "%.*s", (int)(strlen(entry->d_name) - 4), entry->d_name);
                
                snprintf(filepath, sizeof(filepath), "%s/%s", directory, entry->d_name);
                
                struct stat st;
                if (stat(filepath, &st) == 0) {
                    insertItem(t, id, filepath, st.st_size);
                    printf("Loaded clip: %s\n", id);
                }
            }
        }
    }
    closedir(dir);
}

