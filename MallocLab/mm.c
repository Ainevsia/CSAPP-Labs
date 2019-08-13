/*
 * mm.c - teach me how to debug
 *
 * how to deal with sigsegv - gdb
 * explicit free list LIFO
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

#define FREE        0
#define ALLOC       1
#define WSIZE       4
#define DSIZE       8           /* bytes */
#define MINBLOCK    (2 * DSIZE) /* bytes */
#define CHUNKSIZE   (1<<12)     /* bytes */

#define PACK(size, alloc)   ((size) | (alloc))
#define MAX(a, b)           ((a) > (b) ? (a) : (b))
#define MIN(a, b)           ((a) < (b) ? (a) : (b))

/* read and write a word at address p */
#define GET(p)              (*(unsigned int *)(p))
#define PUT(p,val)          (*(unsigned int *)(p) = (val))

/* read the size and allocated fields at address p */
#define GET_SIZE(p)         (GET(p) & ~0x7)
#define GET_ALLOC(p)        (GET(p) & 0x1)

/* when making ptr arimth, make sure cast the ptr type */
/* given the bp, locate the header and footer word */
#define HDRP(bp) ((char *)bp - WSIZE)
#define FTRP(bp) ((char *)bp + GET_SIZE(HDRP(bp)) - DSIZE)

#define NEXT_BLKP(bp) ((char *)bp + GET_SIZE(HDRP(bp)))
#define PREV_BLKP(bp) ((char *)bp - GET_SIZE((char *)bp - DSIZE))

/* free node struct */
#define SET_BP_SUCC(bp, succ)   (*((size_t *)((void *)(bp)        )) = (size_t)(succ))
#define SET_BP_PRED(bp, pred)   (*((size_t *)((void *)(bp) + WSIZE)) = (size_t)(pred))
#define SET_PREV_SUCC(bp, succ) (SET_BP_SUCC(*(size_t *)((void *)(bp) + WSIZE), (succ)))
#define SET_NEXT_PRED(bp, pred) (SET_BP_PRED(*(size_t *)         (bp) ,          (pred)))
#define RELATE_ME(bp)           ((SET_PREV_SUCC((bp), (bp))) & (SET_NEXT_PRED((bp), (bp))))

#define NEXT_FREE_NODE(bp) ((void *)(*(size_t *)(         bp         )))
#define PREV_FREE_NODE(bp) ((void *)(*(size_t *)((char *)(bp) + WSIZE)))

/* get the literal address by ptr bp */
#define ADDRESS(bp) ((*(size_t *)(bp)))

static char * heap_startp;   /* byte */
static char * heap_endp;

/* function declaration */
void pnode(void * bp);
void list_free_node();
void list_blk();
void mm_check_free_node();

int mm_init(void);
void *mm_malloc(size_t size);
static void * find_fit(size_t asize);
static void place(void * bp, size_t asize);
static void * extend_heap(size_t words);
static void * coalesce(void * bp);
void mm_free(void * bp);

static void * attach(void * bp) {
    /* attach to the head of the free list : 3 steps */
    SET_BP_SUCC(bp, ADDRESS(heap_startp));
    SET_BP_PRED(bp, heap_startp);
    RELATE_ME(bp);
    return bp;
}

static void * detach(void * bp) {
    /* bp will be an unreachable node */
    SET_PREV_SUCC(bp, ADDRESS(bp));
    SET_NEXT_PRED(bp, ADDRESS((void *)(bp) + WSIZE));
    return bp;
}

static void * coalesce(void * bp)
{   /* bp is guaranteed to be a free node */
    /* only extend_heap & mmfree will call coalesce function */
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));

    if (prev_alloc && next_alloc) {
        // printf("[CALES] (case 1) starting, bp = %p\n", bp);
        return attach(bp);
    }
    else if (prev_alloc && !next_alloc) {   /* merge behind */
        // printf("[CALES] (case 2) starting, bp = %p\n", bp);
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        detach(NEXT_BLKP(bp));
        PUT(HDRP(bp), PACK(size, FREE));
        PUT(FTRP(bp), PACK(size, FREE));
        attach(bp);
    }
    else if (!prev_alloc && next_alloc) { /* merge front */
        // printf("[CALES] (case 3) starting, bp = %p\n", bp);
        bp = PREV_BLKP(bp);
        size += GET_SIZE(HDRP(bp));
        PUT(HDRP(bp), PACK(size, FREE));
        PUT(FTRP(bp), PACK(size, FREE));
        detach(bp);
        attach(bp);
    }
    else {  /* merge two way */
        // printf("[CALES] (case 4) starting, bp = %p\n", bp);
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(HDRP(NEXT_BLKP(bp)));
        detach(NEXT_BLKP(bp));
        detach(PREV_BLKP(bp));
        bp = PREV_BLKP(bp);
        PUT(HDRP(bp), PACK(size, FREE));
        PUT(FTRP(bp), PACK(size, FREE));
        attach(bp);
    }
    return bp;
}

static void * extend_heap(size_t words)
{
    size_t size = WSIZE * (words % 2 == 1 ? (words + 1) : words);
    char * bp;
    if ((long)(bp = mem_sbrk(size)) == -1)
        return NULL;
    /* bp is pointing to the body */

    /* then the new block is in free state */
    PUT(HDRP(bp), PACK(size, FREE));            /* free block header */
    PUT(FTRP(bp), PACK(size, FREE));            /* free block fotter */
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, ALLOC));   /* epilogue header */
    
    heap_endp = NEXT_BLKP(bp);
    return coalesce(bp);
}

static void * find_fit(size_t asize)
{
    void * bp;
    /* the head pointer cannnot be allocated ! */
    for (bp = NEXT_FREE_NODE(heap_startp); NEXT_FREE_NODE(bp); bp = NEXT_FREE_NODE(bp)) {
        if (GET_SIZE(HDRP(bp)) >= asize)
            return bp;
    }
    return NULL;
}

static void place(void * bp, size_t asize)
{
    size_t block_size = GET_SIZE(HDRP(bp));
    if (block_size < asize + MINBLOCK) { /* dont need to split */
        PUT(HDRP(bp), PACK(block_size, ALLOC));
        PUT(FTRP(bp), PACK(block_size, ALLOC));
        /* remove the node, bp is already a free node */
        detach(bp);
    }
    else { /*split the space and update the free node list*/
        PUT(HDRP(bp), PACK(asize, ALLOC));
        PUT(FTRP(bp), PACK(asize, ALLOC));
        detach(bp);
        bp = NEXT_BLKP(bp);
        PUT(HDRP(bp), PACK(block_size - asize, FREE));
        PUT(FTRP(bp), PACK(block_size - asize, FREE));
        attach(bp);
    }
}


int mm_init(void)
{
    if ((heap_startp = mem_sbrk(5 * DSIZE)) == (void *)-1)
        return -1;
    
    PUT(heap_startp, 0);                                     /* alignment padding*/
    PUT(heap_startp + (1*WSIZE), PACK(MINBLOCK, ALLOC));     /* prologue header*/
    heap_startp += DSIZE;
    
    SET_BP_SUCC(heap_startp, heap_startp + 4*WSIZE);
    SET_BP_PRED(heap_startp, NULL);
    PUT(heap_startp + (2 * WSIZE), PACK(MINBLOCK, ALLOC));     /* prologue fotter*/
    
    PUT(heap_startp + (3 * WSIZE), PACK(MINBLOCK, ALLOC));            /* epilogue header*/
    SET_BP_SUCC(heap_startp + 4*WSIZE, NULL);
    SET_BP_PRED(heap_startp + 4*WSIZE, heap_startp);
    PUT(heap_startp + (6 * WSIZE), PACK(MINBLOCK, ALLOC));
    
    /* extend the heap by chunksize */
    if (extend_heap(CHUNKSIZE/WSIZE) == NULL)
        return -1;

    return 0;
}


void *mm_malloc(size_t size)
{
    if (size == 0) 
        return NULL;
    // printf("[REQUEST] size %ld\n", size);
    size_t asize;   /* adjusted size for alignment */
    if (size < DSIZE) 
        asize = MINBLOCK; 
    else
        asize = ((size + DSIZE + (DSIZE - 1)) / DSIZE) * DSIZE;

    /* search the free list for a fit */
    char * bp = find_fit(asize);
    if (bp != NULL) {
        place(bp, asize);
        // printf("[PLACE] malloc %ld, given %p with size %ld\n", size, bp, asize);
        return bp;
    }

    /* no fit found, get more memory and place the block */
    size_t extend_size = MAX(asize, CHUNKSIZE);

    /* extend_heap receive param as word, not bytes */
    if ((bp = extend_heap(extend_size/WSIZE)) == NULL) 
        return NULL;
    place(bp, asize);
    // printf("[PLACE] malloc %ld, given %p with size %ld\n", size, bp, asize);
    return bp;
}

void mm_free(void * bp) /* LIFO */
{
    size_t size = GET_SIZE(HDRP(bp));
    // printf("[FREE ] %p with size %ld\n", bp, size);
    PUT(HDRP(bp), PACK(size, FREE));
    PUT(FTRP(bp), PACK(size, FREE));

    coalesce(bp);
}

void *mm_realloc(void * bp, size_t size)
{
    if (!bp)
        return mm_malloc(size);
    if (!size) {
        mm_free(bp);
        return NULL;
    }

    size_t src_len = GET_SIZE(HDRP(bp));
    void * dst = mm_malloc(size);
    if (!dst) {
        printf("something horrible happened\n");
        exit(0);
    }
    memcpy(dst, bp, MIN(src_len, size));
    mm_free(bp);
    return dst;
}


void pnode(void * bp) {
    unsigned int * ptr = bp;
    printf("%p <header> size: %d, alloc: %d\n", ptr-1, GET_SIZE((ptr-1)), GET_ALLOC((ptr-1))  );
    printf("%p <succ  > 0x%x\n", ptr  , *ptr      );
    printf("%p <prev  > 0x%x\n", ptr+1, *(ptr+1)  );
    fflush(stdout);
}

void list_free_node() {
    printf("=============list free node start=============\n");
    void * fptr = heap_startp;
    for (; fptr; fptr = NEXT_FREE_NODE(fptr)) {
        pnode(fptr);
        if ((size_t)fptr == ADDRESS(fptr)) {
            printf("error, (size_t)fptr == ADDRESS(fptr)\n");
            exit(0);
        }
    }
    printf("-------------list free node end  -------------\n");
}

void list_blk() {
    printf("=============list_blk start=============\n");
    void * bp;
    for (bp = heap_startp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)) {
        pnode(bp);
    }
    printf("-------------list_blk end  -------------\n");
    
}

void mm_check_free_node() {
    void * bp, * cfn;
    for (bp = heap_startp, cfn = heap_startp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)) {
        if (GET_ALLOC(HDRP(bp)) == FREE && NEXT_FREE_NODE(cfn) != bp) {
            printf("check failed\n");
            list_free_node();
            list_blk();
            exit(0);
        }
        else if (GET_ALLOC(HDRP(bp)) == FREE && NEXT_FREE_NODE(cfn) == bp)
            cfn = NEXT_FREE_NODE(cfn);
    }

    for (bp = heap_endp, cfn = heap_endp; bp != heap_startp; bp = PREV_BLKP(bp)) {
        if (GET_ALLOC(HDRP(bp)) == FREE && PREV_FREE_NODE(cfn) != bp) {
            printf("reverse check failed\n");
            list_free_node();
            list_blk();
            exit(0);
        }
        else if (GET_ALLOC(HDRP(bp)) == FREE && PREV_FREE_NODE(cfn) == bp)
            cfn = PREV_FREE_NODE(cfn);
    }
}
