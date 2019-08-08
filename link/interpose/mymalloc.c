#ifdef compiletime
#include <stdio.h>
#include <malloc.h>

void *mymalloc(size_t size)
{
    void *p = malloc(size);
    printf("malloc(%d)= %p\n", (int)size, p);
    return p;
}

void myfree(void *ptr)
{
    free(ptr);
    printf("free %p\n",  ptr);
}
#endif

#ifdef linktime
#include <stdio.h>

void *__real_malloc(size_t size);
void __real_free(void *ptr);

void *__wrap_malloc(size_t size)
{
    void *p = __real_malloc(size);
    printf("malloc(%d)= %p\n", (int)size, p);
    return p;
}

void __wrap_free(void *ptr)
{
    __real_free(ptr);
    printf("free %p\n",  ptr);
}
#endif