#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stddef.h>

#define TABLE_SIZE 128

typedef struct Item {
    char* id;
    char* path;
    size_t size;
    struct Item* next;
} Item;

typedef struct Table {
    Item* items[TABLE_SIZE];
} Table;

int hash(const char* id);
Table* createTable(void);
void insertItem(Table* t, const char* id, const char* path, size_t size);
Item* getItem(Table* t, const char* id);
void freeTable(Table* t);
void loadClipsFromDir(Table* t, const char* directory);

#endif
