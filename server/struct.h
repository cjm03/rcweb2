#ifndef STRUCT_H
#define STRUCT_H

#include <stddef.h>

#define TBLSZ 128

typedef struct Clip {
    char* id;
    char* filepath;
    size_t filesize;
    struct Clip* next;
} Clip;

typedef struct Table {
    Clip* buckets[TBLSZ];
} Table;

#endif
