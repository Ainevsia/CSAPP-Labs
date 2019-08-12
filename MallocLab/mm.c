/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "Ficus",
    /* First member's full name */
    "Ainevsia",
    /* First member's email address */
    "zhipengxu@sjtu.edu.cn",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""
};

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)

#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

#define WSIZE       4
#define DSIZE       8           /* bytes */
#define CHUNKSIZE   (1<<12)     /* bytes */
#define MINBLOCK    (2 * DSIZE) /* bytes */
#define ALLOC       1
#define FREE        0

#define PACK(size, alloc) ((size) | (alloc))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

/* read and write a word at address p */
#define GET(p)      (*(unsigned int *)(p))
#define PUT(p,val)  (*(unsigned int *)(p) = (val))

/* read the size and allocated fields at address p */
#define GET_SIZE(p)     (GET(p) & ~0x7)
#define GET_ALLOC(p)    (GET(p) & 0x1)

// when making ptr arimth, make sure cast the ptr type
/* given the bp, locate the header and footer word */
#define HDRP(bp) ((char *)bp - WSIZE)
#define FTRP(bp) ((char *)bp + GET_SIZE(HDRP(bp)) - DSIZE)

#define NEXT_BLKP(bp) ((char *)bp + GET_SIZE(HDRP(bp)))
#define PREV_BLKP(bp) ((char *)bp - GET_SIZE((char *)bp - DSIZE))

static char * heap_listp;
static char * heap_maxp;

void pheap()
{
    
    void * bp;
    printf("=========== mem layout start ==============\n");
    for (bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)) {
        printf("from %p to %p size %d status %d\n", bp, FTRP(bp), GET_SIZE(HDRP(bp)), GET_ALLOC(HDRP(bp)));
    }
    printf("----------- mem layout end ----------------\n");
}

/* 
 * coalesce - four cases to make bigger memory
 */
static void * coalesce(void * bp)
{
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));
    // getsize mistyped : GET_ALLOC
    if (prev_alloc && next_alloc)
        return bp;
    else if (prev_alloc && !next_alloc) {   /* merge behind */
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(bp), PACK(size, FREE));
        PUT(FTRP(bp), PACK(size, FREE));
        // printf("[CALES] (case 2) merging %p to %p (size %d)\n", HDRP(bp), FTRP(bp), GET_SIZE(HDRP(bp)));
    }
    else if (!prev_alloc && next_alloc) { /* merge front */
        bp = PREV_BLKP(bp);
        size += GET_SIZE(HDRP(bp));
        PUT(HDRP(bp), PACK(size, FREE));
        PUT(FTRP(bp), PACK(size, FREE));
        // printf("[CALES] (case 3) merging %p to %p (size %d)\n", HDRP(bp), FTRP(bp), GET_SIZE(HDRP(bp)));
    }
    else {  /* merge two way */
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(HDRP(NEXT_BLKP(bp)));
        bp = PREV_BLKP(bp);
        PUT(HDRP(bp), PACK(size, FREE));
        PUT(FTRP(bp), PACK(size, FREE));
        // printf("[CALES] (case 4) merging %p to %p (size %d)\n", HDRP(bp), FTRP(bp), GET_SIZE(HDRP(bp)));
    }
    return bp;
}

/* 
 * extend_heap - extend the heap by 'words' words.
 */
static void * extend_heap(size_t words)
{
    size_t size = WSIZE * (words % 2 == 1 ? (words + 1) : words);
    char * bp;
    if ((long)(bp = mem_sbrk(size)) == -1)
        return NULL;

    /* then the new block is in free state */
    PUT(HDRP(bp), PACK(size, FREE));            /* free block header */
    PUT(FTRP(bp), PACK(size, FREE));            /* free block fotter */
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, ALLOC));   /* epilogue header */
    heap_maxp = HDRP(NEXT_BLKP(bp));
    return coalesce(bp);
}

static void * find_fit(size_t asize)
{
    void * bp;
    for (bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)) {
        if (GET_ALLOC(HDRP(bp)) == FREE && GET_SIZE(HDRP(bp)) >= asize)
            return bp;
    }
    return NULL;
}

static void place(void * bp, size_t asize)
{
    /* consider the split case */
    size_t block_size = GET_SIZE(HDRP(bp));
    if (block_size < asize + MINBLOCK) {
        /* too small, do not need to split */
        PUT(HDRP(bp), PACK(block_size, ALLOC));
        PUT(FTRP(bp), PACK(block_size, ALLOC));
        // PUT(HDRP(bp), PACK(asize, ALLOC));
        // PUT(FTRP(bp), PACK(asize, ALLOC));
        /* mistake here : if too small, the whole block is his */
        /* the two cases certainly does not have the same operation */
        /* otherwise, the next zero block will be taken as the next block ! */
    }
    else {
        PUT(HDRP(bp), PACK(asize, ALLOC));
        PUT(FTRP(bp), PACK(asize, ALLOC));
        bp = NEXT_BLKP(bp);
        PUT(HDRP(bp), PACK(block_size - asize, FREE));
        PUT(FTRP(bp), PACK(block_size - asize, FREE));
    }
}

/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    /* create the empty heap */
    if ((heap_listp = mem_sbrk(4*WSIZE)) == (void *)-1)
        // there is a problem in performing initialization
        return -1;
    
    PUT(heap_listp, 0);                                 /* alignment padding*/
    PUT(heap_listp + (1*WSIZE), PACK(DSIZE, ALLOC));    /* prologue header*/
    PUT(heap_listp + (2*WSIZE), PACK(DSIZE, ALLOC));    /* prologue fotter*/
    PUT(heap_listp + (3*WSIZE), PACK(0, ALLOC));        /* epilogue header*/
    
    /* move heap_ptr to the bp place */
    heap_listp += DSIZE;

    /* extend the heap by chunksize */
    if (extend_heap(CHUNKSIZE/WSIZE) == NULL)
        return -1;
    return 0;
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    if (size == 0) 
        return NULL;

    size_t asize;   /* adjusted size for alignment */
    if (size < DSIZE) /* do not allow malloc zero block*/
        asize = MINBLOCK;  // why here is dsize * 2 ?
        // the minimun head + footer = dsize (8 bytes)
    else
        asize = ((size + DSIZE + (DSIZE - 1)) / DSIZE) * DSIZE;

    /* search the free list for a fit */
    char * bp = find_fit(asize);
    if (bp != NULL) {
        /* please make sure find enough space */
        place(bp, asize);
        return bp;
    }

    /* no fit found, get more memory and place the block */
    size_t extend_size = MAX(asize, CHUNKSIZE);

    /* extend_heap receive param as word, not bytes */
    if ((bp = extend_heap(extend_size/WSIZE)) == NULL) 
        return NULL;
    place(bp, asize);
    return bp;
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void * bp) /* finished */
{
    if (GET_ALLOC(HDRP(bp)) != ALLOC || GET_ALLOC(FTRP(bp)) != ALLOC)
        return;
    size_t size = GET_SIZE(HDRP(bp));
    PUT(HDRP(bp), PACK(size, FREE));
    PUT(FTRP(bp), PACK(size, FREE));
    coalesce(bp);
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
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


void *ori_mm_malloc(size_t size)
{
    int newsize = ALIGN(size + SIZE_T_SIZE);
    void *p = mem_sbrk(newsize);
    if (p == (void *)-1)
	    return NULL;
    else {
        *(size_t *)p = size;
        return (void *)((char *)p + SIZE_T_SIZE);
    }
}

void *ori_mm_realloc(void *ptr, size_t size)
{
    void *oldptr = ptr;
    void *newptr;
    size_t copySize;
    
    newptr = mm_malloc(size);
    if (newptr == NULL)
      return NULL;
    copySize = *(size_t *)((char *)oldptr - SIZE_T_SIZE);
    if (size < copySize)
      copySize = size;
    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);
    return newptr;
}









