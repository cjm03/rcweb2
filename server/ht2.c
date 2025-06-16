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
#include "struct.h"


static Table* table;

/* hash *********************************************************/

unsigned long djb2(const char* s)
{
    unsigned long hash = 5381;
    int c;
    while ((c = *s++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash % TBLSZ;
}

/* init *********************************************************/

void initTable(void)
{
    if (table == NULL) {
        printf("TABLE IS NULL\n");
        return;
    }
    for (int i = 0; i < TBLSZ; ++i) {
        table->buckets[i] = NULL;
    }
}

void insertClip(const char* id, const char* filepath, size_t filesize)
{
    unsigned long index = djb2(id);
    Clip* newclip = calloc(1, sizeof(Clip));
    if (newclip == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }
    newclip->id = strdup(id);
    newclip->filepath = strdup(filepath);
    newclip->filesize = filesize;

    printf("FILESIZE ASSIGNED\n");

    newclip->next = table->buckets[index];

    printf("PTR TO NEXT ASSIGNED CURRENT HASH REP. INDEX\n");

    table->buckets[index] = newclip;

    printf("BUCKET AT HASH INDEX ASSIGNED NEWCLIP\n");
}

Clip* getClip(const char* id)
{
    unsigned long index = djb2(id);
    Clip* cur = table->buckets[index];
    while (cur) {
        if (strcmp(cur->id, id) == 0) {
            return cur;
        }
        cur = cur->next;
    }
    return NULL;
}

// int ismp4(const char* id)
// {
//     const char* ext = strrchr(id, '.');
//     return ext && strcmp(ext, ".mp4") == 0;
// }

void loadFromDir(const char* directory)
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
                snprintf(id, sizeof(id), "%.*s", (int)(strlen(entry->d_name) - 4), entry->d_name);

                char filepath[512];
                snprintf(filepath, sizeof(filepath), "%s/%s", directory, entry->d_name);

                struct stat st;
                if (stat(filepath, &st) == 0) {
                    insertClip(id, filepath, st.st_size);
                    printf("Loaded clip: %s\n", id);
                }
            }
        }
    }
    closedir(dir);
}

void freeTable(void)
{
    for (int i = 0; i < TBLSZ; i++) {
        Clip* cur = table->buckets[i];
        while (cur) {
            Clip* next = cur->next;
            free(cur->id);
            free(cur->filepath);
            free(cur);
            cur = next;
        }
    }
}

void iterateClips(void (*callback)(Clip *clip, void *ctx), void *ctx)
{
    for (int i = 0; i < TBLSZ; i++) {
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
    printf("%s - %s (%ld bytes)\n", clip->id, clip->filepath, clip->filesize);
}

void printTable(void)
{
    printf("Start Table:\n");
    for (int i = 0; i < TBLSZ; i++) {
        Clip* iter = table->buckets[i];
        if (iter == NULL) {
            printf("---EMPTY-BUCKET---\n");
        } else {
            printf("\nid: %s\npath: %s\nsize: %ld\n\n", iter->id, iter->filepath, iter->filesize);
        }
    }
}

