#include <stdio.h>

#include "ht2.h"

int main(void)
{
    Table table = {0};
    scanDir(&table, "../media");
    printf("Indexed clips:\n");
    iterateClips(&table, printClip, NULL);
    freeTable(&table);
    return 0;
}
