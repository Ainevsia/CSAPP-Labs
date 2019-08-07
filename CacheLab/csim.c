#include "cachelab.h"
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <math.h>
#define ADDRBIT 64
#define TRUE 1
#define FALSE 0

int MISS, EVICTION, HIT;
char * filepath;

struct Line {
    int aval;
    unsigned long tag;
    int lastused;
};

struct Set {
    struct Line * line;
};

struct Cache{
    int setnumber;
    int linenumber; 
    int blocksize;
    unsigned int setbit;
    unsigned int blockbit;
    unsigned int tagbit;
    struct Set * set;
} cache;

void pharse(int argc, char *argv[]) {
    int ch;
    while((ch = getopt(argc, argv, "s:E:b:t:")) != -1) {
        switch (ch) {
            case 's':
                cache.setbit = atoi(optarg);
                cache.setnumber = pow(2,cache.setbit);
                break;
            case 'E':
                cache.linenumber = atoi(optarg);
                break;
            case 'b':
                cache.blockbit = atoi(optarg);
                cache.blocksize = pow(2,cache.blockbit);
                break;
            case 't':
                filepath = optarg;
                break;
            default :
                printf("Usage: ./csim-ref [-hv] -s <num> -E <num> -b <num> -t <file>\n");
        }
    }
}

void debug() {
    for (int i = 0; i < cache.setnumber; i++)
    {
        printf("==set %d==\n", i);
        for (int j = 0; j < cache.linenumber; j++)
        {
            printf("%d %d %lx\n", cache.set[i].line[j].aval, cache.set[i].line[j].lastused, cache.set[i].line[j].tag);
        }
    }
}

int main(int argc, char *argv[])
{
    pharse(argc, argv);
    cache.tagbit = ADDRBIT - cache.setbit - cache.blockbit;
    cache.set = (struct Set *)malloc(sizeof(struct Set)*cache.setnumber);
    for (int i=0; i<cache.setnumber; i++) {
        cache.set[i].line = (struct Line *)malloc(sizeof(struct Line)*cache.linenumber);
        for (int j=0; j<cache.linenumber; j++) {
            cache.set[i].line[j].aval = FALSE;
        }
    }
    freopen(filepath, "r", stdin);
    /* scanf returns -1 when fails */
    char head, c;
    unsigned long addr;
    int size;
    // printf("s: %d\tline: %d\tblock: %d\n",cache.setnumber,cache.linenumber,cache.blocksize);
    while (scanf("%c", &head) != -1) {
        if (head == ' ')
            scanf("%c %lx,%d\n", &c, &addr, &size);
        else {
            c = head;
            scanf("%lx,%d\n", &addr, &size);
        }
        if (c != 'I')
            printf("%c %lx,%d", c, addr, size);

        /* operation start */
        unsigned long setidx = (addr << cache.tagbit) >> (cache.tagbit + cache.blockbit);
        unsigned long tag = addr >> (cache.blockbit + cache.setbit);
        // printf("setid: %lx\ttag: %lx\n", setidx, tag);
        
        switch (c) {
            case 'S':
            case 'M':
            case 'L':
                printf(" ");
                int hit = FALSE;
                for (int i=0; i<=cache.linenumber; i++) { /* search for hit */
                    struct Line *curline = &cache.set[setidx].line[i];
                    if (curline->tag == tag && curline->aval == TRUE) { /* hit case */
                        printf("hit ");
                        /* update other lines' lastuse in the same set */
                        for (int i = 0; i < cache.linenumber; i++) {
                            if (cache.set[setidx].line[i].aval == TRUE) {
                                cache.set[setidx].line[i].lastused++;
                            }
                        }
                        curline->lastused = 1;
                        hit = TRUE;
                        HIT++;
                        break;
                    }
                }
                if (!hit) {
                    MISS++;
                    printf("miss ");
                    /* LRU (least-recently used) replacement policy  */
                    int evict = TRUE;
                    for (int i = 0; i < cache.linenumber; i++) /* search for none used */ {
                        if (cache.set[setidx].line[i].aval == 0) /* found not used */ {
                            evict = FALSE;
                            cache.set[setidx].line[i].tag = tag;
                            cache.set[setidx].line[i].aval = TRUE;
                            /* update other lines' lastuse in the same set */
                            for (int i = 0; i < cache.linenumber; i++) {
                                if (cache.set[setidx].line[i].aval != FALSE) {
                                    cache.set[setidx].line[i].lastused++;
                                }
                            }
                            cache.set[setidx].line[i].lastused = 1;
                            break;
                        }
                    }
                    if (evict) {
                        struct Line* lptr = &cache.set[setidx].line[0];
                        int lru = lptr->lastused;
                        for (int i = 0; i < cache.linenumber; i++)
                        {
                            if (cache.set[setidx].line[i].lastused > lru)
                            {
                                lru = cache.set[setidx].line[i].lastused;
                                lptr = &cache.set[setidx].line[i];
                            }
                        }
                        lptr->tag = tag;
                        lptr->lastused = 1;
                        printf("eviction ");

                        EVICTION++;
                        /* update other lines' lastuse in the same set */
                        for (int i = 0; i < cache.linenumber; i++) {
                            if (cache.set[setidx].line[i].tag != tag && cache.set[setidx].line[i].aval == TRUE) {
                                cache.set[setidx].line[i].lastused++;
                            }
                        }
                    }
                }
                if ((c == 'M')) {
                    HIT++;
                    printf("hit ");
                }
                break;
            default:
                continue;
        }
        printf("\n");
        // debug();
    }
    printSummary(HIT, MISS, EVICTION);
    return 0;
}
