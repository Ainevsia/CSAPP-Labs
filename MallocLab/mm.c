/*
 * mm.c - teach me how to debug
 *
 * how to deal with sigsegv - gdb
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

static char * heap_startp;   /* byte */
static char * heap_endp;
static size_t malloc_cnt = 0;
#define ROUND 200

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

static void * coalesce(void * bp)
{
    /* only extend_heap & mmfree will call coalesce function */
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));

    if (prev_alloc && next_alloc) {
        /* take the risk of being a lonely node*/
        // printf("[CALES] (case 1) starting, bp = %p\n", bp);
        
        void * next_bp = NEXT_BLKP(bp);
        if (GET_SIZE(HDRP(next_bp)) == 0)
            next_bp = NULL;
        else {
            /* find the next free node in the list, search by blkp */
            for (next_bp = NEXT_BLKP(bp); GET_ALLOC(HDRP(next_bp)) == ALLOC; next_bp = NEXT_BLKP(next_bp))
                if (GET_SIZE(HDRP(next_bp)) == 0) {
                    next_bp = NULL;
                    break;
                }
        }
        if (next_bp == NULL) 
            next_bp = heap_endp;
        
        void * prev_bp;
        /* find the prev free node in the list */
        for (prev_bp = PREV_BLKP(bp); GET_ALLOC(HDRP(prev_bp)) == ALLOC; prev_bp = PREV_BLKP(prev_bp)) {
            if (prev_bp == heap_startp) {
                prev_bp = NULL;
                break;
            }
        }
        if (prev_bp == NULL) 
            prev_bp = heap_startp;
        SET_BP_SUCC(bp, next_bp);
        SET_BP_PRED(bp, prev_bp);
        RELATE_ME(bp);
        return bp;
    }
    else if (prev_alloc && !next_alloc) {   /* merge behind */
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        memcpy(bp, NEXT_BLKP(bp), DSIZE);
        RELATE_ME(bp);
        PUT(HDRP(bp), PACK(size, FREE));
        PUT(FTRP(bp), PACK(size, FREE));
        // printf("[CALES] (case 2) merging %p to %p (size %d)\n", HDRP(bp), FTRP(bp), GET_SIZE(HDRP(bp)));
    }
    else if (!prev_alloc && next_alloc) { /* merge front */
        // printf("[CALES] (case 3) merging bp = %p \n", (bp));
        bp = PREV_BLKP(bp);
        size += GET_SIZE(HDRP(bp));
        PUT(HDRP(bp), PACK(size, FREE));
        PUT(FTRP(bp), PACK(size, FREE));
    }
    else {  /* merge two way */
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(HDRP(NEXT_BLKP(bp)));
        memcpy(PREV_BLKP(bp), NEXT_BLKP(bp), WSIZE);
        bp = PREV_BLKP(bp);
        RELATE_ME(bp);
        PUT(HDRP(bp), PACK(size, FREE));
        PUT(FTRP(bp), PACK(size, FREE));
        // printf("[CALES] (case 4) merging %p to %p (size %d)\n", HDRP(bp), FTRP(bp), GET_SIZE(HDRP(bp)));
    }
    return bp;
}

static void * extend_heap(size_t words)
{
    size_t size = WSIZE * (words % 2 == 1 ? (words + 1) : words);
    char * bp;
    if ((long)(bp = mem_sbrk(size)) == -1)
        return NULL;
    /* bp points to the epilogue head */
    bp -= 2 * WSIZE;

    /* then the new block is in free state */
    PUT(HDRP(bp), PACK(size, FREE));            /* free block header */
    PUT(FTRP(bp), PACK(size, FREE));            /* free block fotter */
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, ALLOC));   /* epilogue header */
    
    /* you should not let the origin heapend points to the current end */
    /* instead, let the pre next point to the current end*/
    SET_PREV_SUCC(bp, NEXT_BLKP(bp));
    SET_BP_SUCC(NEXT_BLKP(bp), NULL);
    SET_BP_SUCC(bp, NEXT_BLKP(bp));
    SET_NEXT_PRED(bp, PREV_FREE_NODE(bp));
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
        // if (NEXT_FREE_NODE(NEXT_FREE_NODE(bp)) == bp) {
        //     printf("reversed link node !!!!! DUMP\n");
        //     exit(0);
        // }
    }
    return NULL;
}

static void place(void * bp, size_t asize)
{
    // if (bp == heap_startp) {
    //     // list_blk();
    //     printf("bp == heap !!!!\n");
    //     list_free_node();
    // }
    size_t block_size = GET_SIZE(HDRP(bp));
    if (block_size < asize + MINBLOCK) { /* dont need to split */
        PUT(HDRP(bp), PACK(block_size, ALLOC));
        PUT(FTRP(bp), PACK(block_size, ALLOC));
        /* remove the node, bp is already a free node */
        SET_NEXT_PRED(bp, PREV_FREE_NODE(bp));
        SET_PREV_SUCC(bp, NEXT_FREE_NODE(bp));
    }
    else { /*split the space and update the free node list*/
        PUT(HDRP(bp), PACK(asize, ALLOC));
        PUT(FTRP(bp), PACK(asize, ALLOC));
        memcpy(NEXT_BLKP(bp), bp, DSIZE);
        
        bp = NEXT_BLKP(bp);
        PUT(HDRP(bp), PACK(block_size - asize, FREE));
        PUT(FTRP(bp), PACK(block_size - asize, FREE));
        RELATE_ME(bp);
    }
}


int mm_init(void)
{
    if ((heap_startp = mem_sbrk(4 * DSIZE)) == (void *)-1)
        return -1;
    
    PUT(heap_startp, 0);                                     /* alignment padding*/
    PUT(heap_startp + (1*WSIZE), PACK(MINBLOCK, ALLOC));     /* prologue header*/
    /* move heap_ptr to the bp place */
    heap_startp += DSIZE;
    
    SET_BP_SUCC(heap_startp, heap_startp + 4*WSIZE);
    SET_BP_PRED(heap_startp, NULL);
    PUT(heap_startp + (2 * WSIZE), PACK(MINBLOCK, ALLOC));     /* prologue fotter*/
    
    PUT(heap_startp + (3 * WSIZE), PACK(0, ALLOC));            /* epilogue header*/
    SET_BP_SUCC(heap_startp + 4*WSIZE, NULL);
    SET_BP_PRED(heap_startp + 4*WSIZE, heap_startp);
    
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
    malloc_cnt += 1;
    
    // mm_check_free_node();
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
        // list_free_node();
        // mm_check_free_node();
        return bp;
    }

    /* no fit found, get more memory and place the block */
    size_t extend_size = MAX(asize, CHUNKSIZE);

    /* extend_heap receive param as word, not bytes */
    if ((bp = extend_heap(extend_size/WSIZE)) == NULL) 
        return NULL;
    place(bp, asize);
    // printf("[PLACE] malloc %ld, given %p with size %ld\n", size, bp, asize);
    // list_free_node();
    // mm_check_free_node();
    return bp;
}

void mm_free(void * bp) 
{
    if (GET_ALLOC(HDRP(bp)) != ALLOC || GET_ALLOC(FTRP(bp)) != ALLOC)
        return;
    size_t size = GET_SIZE(HDRP(bp));
    // printf("[FREE ] %p\n with size %ld", bp, size);
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
