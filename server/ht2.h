#ifndef HT2_H
#define HT2_H

#include <time.h>
#include <stdbool.h>
#include <stddef.h>

#include "struct.h"


/* prototypes ***************************************************/

// takes in a string that should be a clip id from a Clip* structure, returns a hash
unsigned long djb2(const char* id);

// populates the table in hashtable.c with TABLE_SIZE elements initialized to NULL
void initTable(void);

// takes in an id string, filename, and size of the file
// calls djb2 to get a hash from the id parameter and assigns it to index
// malloc's a block of size sizeof(Clip), assigns it to variable newclip of type Clip*
// strdup's the id and filename into the newclip, assigns filesize passed to function
// assigns the pointer to the next element with the hash assigned to index
// assigns the table element at the index of the hash with newclip
// *** essentially, the newclip->next is its own index until a new clip is inserted
void insertClip(const char* id, const char* filepath, size_t filesize);

// takes in a string that should be a clip id from a Clip* structure
// gets the hash of the id and stores it in index
// creates a Clip* cur and assigns it with the element at the hash in the table
// enters a while loop unless cur is empty, which would return NULL and exit the func
// compares the id of cur with the id passed to the function, returns the structure if they match
// otherwise, cur is assigned its pointer to next, repeats until found or exits
Clip* getClip(const char* id);



void loadFromDir(const char* directory);

// iterates through the table freeing each clip structure until table is empty
void freeTable(void);

// still unsure how exactly this works.
void iterateClips(void (*callback)(Clip* clip, void *ctx), void *ctx);


void printClip(Clip* clip, void* ctx);


void printTable(void);
// unsigned long djb2(const char* s);
// void initHashTable(void);
// void displayHashTable(void);
// htClip* htNewClip(const char* id, const char* path);
// bool htInsert(htClip* c);
// htClip* htSearch(const char* id);
// htClip* htDelete(char* id);

#endif
