#include "zad1.h"


void block_init(block * blk, size_t size) 
{
    if (size <= 0 || blk == NULL) return;

    blk->fline = (char **) calloc(size, sizeof(char *));

    if (blk->fline == NULL) return;

    blk->size = size;    
}


void block_clear(block * blk) 
{
    if (blk == NULL) return;

    for (size_t i = 0; i < blk->size; ++i) 
        if (*(blk->fline + i) != NULL) free(*(blk->fline + i));
    
    free(blk->fline);
    blk = NULL;
}


blockch * blockch_create(size_t size) 
{
    if (size <= 0) return NULL;

    blockch * blkc = (blockch *) malloc(sizeof(blockch));

    blkc->blkarr = (block * ) calloc(size, sizeof(block));

    if (blkc->blkarr == NULL) 
    {
        free(blkc);
        return NULL;
    }

    blkc->size = size;

    // czy zerownie jest potrzebne, jeżeli korzystamy z calloca? 
    // nie
    // for (size_t i = 0; i < size; ++i) (blkc->blkarr + i)->fline = NULL;

    return blkc;
}


void blockch_delete(blockch * blkc) 
{
    if (blkc == NULL) return;

    free(blkc->blkarr);
    free(blkc);
}


void blockch_delete_all(blockch * blkc) 
{
    if (blkc == NULL) return;

    for (size_t i = 0; i < blkc->size; ++i) block_clear(blkc->blkarr + i);
    free(blkc->blkarr);
    free(blkc);
}


void block_insert_at(block * blk, size_t idx, char * line)
{
    if (blk == NULL || idx < 0 || idx >= blk->size) return;

    // dodatkowy bajt na 'nulla'
    *(blk->fline + idx) = (char *) calloc(strlen(line) + 1, sizeof(char));

    if (*(blk->fline + idx) == NULL) return;

    strcpy(*(blk->fline + idx), line);
}


void block_remove_from(block * blk, size_t idx) 
{
    if (blk == NULL || idx < 0 || idx >= blk->size || *(blk->fline + idx) == NULL) return;

    free(*(blk->fline + idx));
    *(blk->fline + idx) = NULL;
}


size_t block_linecount(block * blk) 
{
    if (blk == NULL) return 0;

    size_t count = 0;
    for (size_t i = 0; i < blk->size; ++i) if (*(blk->fline + i) != NULL) ++count;

    return count; 
}


void block_print(block * blk, size_t idx)
{
    if (blk == NULL || blk->fline == NULL) return;

    printf("BLOCK %ld AT %ld\n", idx, (long) blk);
    for (size_t i = 0; i < blk->size; ++i) printf("L%-3ld: %s\n", i, *(blk->fline + i));
}


void blockch_print(blockch * blkc)
{
    if (blkc == NULL || blkc->blkarr == NULL) return;

    printf("=====================================\n");
    printf("BLOCK CHAIN AT %ld\n", (long) blkc);
    printf("=====================================\n");

    for (size_t i = 0; i < blkc->size; ++i) block_print(blkc->blkarr + i, i);

    printf("=====================================\n");
}