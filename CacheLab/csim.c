#include "cachelab.h"
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <math.h>
#define ADDRBIT 64
#define TRUE 1
#define FALSE 0

int miss, eviction, hit;
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
    printf("s: %d\tline: %d\tblock: %d\n",cache.setnumber,cache.linenumber,cache.blocksize);
    while (scanf("%c", &head) != -1) {
        if (head == ' ')
            scanf("%c %lx,%d\n", &c, &addr, &size);
        else {
            c = head;
            scanf("%lx,%d\n", &addr, &size);
        }
        printf("%c %lx,%d\t", c, addr, size);

        /* operation start */
        // addr >>= cache.blockbit;
        unsigned long setidx = (addr << cache.tagbit) >> (cache.tagbit + cache.blockbit);
        unsigned long tag = addr >> (cache.blockbit + cache.setbit);
        printf("%lx\t%lx\n", setidx, tag);
        
        switch (c) {
            case 'S':
                break;
            case 'L':
                for (int i=0; i<=cache.linenumber; i++) {
                    struct Line curline = cache.set[setidx].line[i];
                    if (curline.tag == tag && curline.aval == TRUE) hit++;
                }
                break;
            case 'M':
                break;
            default:
                continue;
        }
    }
   
    printSummary(miss, eviction, hit);
    return 0;
}
