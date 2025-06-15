/*
 *  hashtable.c
 *
 *  Store video metadata for fast lookup
*/

#include <stdio.h>
#include <time.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "ht2.h"


// static Clip* Table[TABLE_SIZE];

/* hash *********************************************************/

unsigned long djb2(const char* s)
{
    unsigned long hash = 5381;
    int c;
    while ((c = *s++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash % TABLE_SIZE;
}

/* functions ****************************************************/

void insertClip(Table* table, const char* name, const char* path, size_t filesize)
{
    unsigned long index = djb2(name);
    Clip* newclip = malloc(sizeof(Clip));
    newclip->name = strdup(name);
    newclip->path = strdup(path);
    newclip->filesize = filesize;
    newclip->next = table->buckets[index];
    table->buckets[index] = newclip;
}

Clip* getClip(Table* table, const char* name)
{
    unsigned long index = djb2(name);
    Clip* cur = table->buckets[index];
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            return cur;
        }
        cur = cur->next;
    }
    return NULL;
}

int ismp4(const char* name)
{
    const char* ext = strrchr(name, '.');
    return ext && strcmp(ext, ".mp4") == 0;
}

void scanDir(Table* table, const char* directory)
{
    DIR* dir = opendir(directory);
    if (!dir) {
        perror("opendir");
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(dir))) {
        if (entry->d_type == DT_REG) {
            if (ismp4(entry->d_name)) {
                char name[256] = {0};
                snprintf(name, sizeof(name), "%.*s", (int)(strlen(entry->d_name) - 4), entry->d_name);
                char path[512];
                snprintf(path, sizeof(path), "%s/%s", directory, entry->d_name);
                struct stat st;
                if (stat(path, &st) == 0) {
                    insertClip(table, entry->d_name, path, st.st_size);
                    printf("Loaded clip: %s\n", name);
                }
            }
        }
    }
    closedir(dir);
}

void freeTable(Table* table)
{
    for (int i = 0; i < TABLE_SIZE; i++) {
        Clip* cur = table->buckets[i];
        while (cur) {
            Clip* next = cur->next;
            free(cur->name);
            free(cur->path);
            free(cur);
            cur = next;
        }
    }
}

void iterateClips(Table* table, void (*callback)(Clip *clip, void *ctx), void *ctx)
{
    for (int i = 0; i < TABLE_SIZE; i++) {
        Clip* clip = table->buckets[i];
        while (clip) {
            callback(clip, ctx);
            clip = clip->next;
        }
    }
}

void printClip(Clip* clip, void* ctx)
{
    (void)ctx;
    printf("%s - %s (%ld bytes)\n", clip->name, clip->path, clip->filesize);
}

// void displayHashTable(void)
// {
//     printf("Start\n");
//     for (int i = 0; i < TABLE_SIZE; i++) {
//         if (htTable[i] == NULL) {
//             printf("\tK: -------- P: --------\n");
//         } else {
//             printf("\tK: %s P: --------\n", htTable[i]->key);
//         }
//     }
//     printf("End\n");
// }

/* main ********************************************************************/







