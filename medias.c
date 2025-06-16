#include <stdio.h>
#include <string.h>
#include <math.h>

#define TBLSZ 128

unsigned long djb2(const char* s)
{
    unsigned long hash = 5381;
    int c;
    while ((c = *s++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash % TBLSZ;
}

int hthash(const char* s)
{
    long hash = 0;
    int len = strlen(s);
    for (int i = 0; i < len; i++) {
        hash += (long)pow(151, len - (i + 1)) * s[i];
        hash = hash % TBLSZ;
    }
    return (int)hash;
}

int main(void)
{
    char* c1 = "clip_001.mp4";
    char* c2 = "clip_002.mp4";
    char* c3 = "clip_003.mp4";
    char* c4 = "desktop2023.04.09_2.52.mp4";
    char* c5 = "pubg2023.01.15_5.37.mp4";
    char* c6 = "pubg2023.01.15_11.51.mp4";
    char* c7 = "pubg2023.01.16_8.52.mp4";
    char* c8 = "pubg2023.01.25_6.35.mp4";
    char* c9 = "pubg2023.01.28_7.20.mp4";
    char* c10 = "pubg2023.12.01_10.33.mp4";
    char* c11 = "RUST2024.12.16-06.01.30.02.DVR-Trim.mp4";

    printf("c1: %d\n", hthash(c1));
    printf("c2: %d\n", hthash(c2));
    printf("c3: %d\n", hthash(c3));
    printf("c4: %d\n", hthash(c4));
    printf("c5: %d\n", hthash(c5));
    printf("c6: %d\n", hthash(c6));
    printf("c7: %d\n", hthash(c7));
    printf("c8: %d\n", hthash(c8));
    printf("c9: %d\n", hthash(c9));
    printf("c10: %d\n", hthash(c10));
    printf("c11: %d\n", hthash(c11));
    return 0;
}
