#define malloc(x) mymalloc(x)
#define free(x) myfree(x)

void *mymalloc(size_t size);

void myfree(void *ptr);
