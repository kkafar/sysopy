#include "libzad1.h"
#include <stdlib.h>

struct block 
{
    char *** blkptr;
    size_t size;
    size_t csize; // current size
}; 


struct block * block_create(size_t n) 
{
    struct block * bkptr = calloc(1, sizeof(struct block));
    if (bkptr == NULL) exit(1);

    bkptr->size = n;
    bkptr->csize = 0;

    bkptr->blkptr = calloc(n, sizeof(char **));

    return bkptr;
}


