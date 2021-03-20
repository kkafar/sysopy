#include "zad1.h"
#include <stdio.h>
#include <string.h>


int main(void) 
{
    char * data = "abecadlozpiecaspadloioziemiesiestluklo";
    char * data2 = "costam";

    blockch * blkc = blockch_create(4);

    block_init(blkc->blkarr, 2);
    printf("niepuste linie: %ld\n", block_linecount(blkc->blkarr));

    block_insert_at(blkc->blkarr, 0, data);
    block_insert_at(blkc->blkarr, 1, data2);

    printf("niepuste linie: %ld\n", block_linecount(blkc->blkarr));

    blockch_print(blkc);

    block_remove_from(blkc->blkarr, 1);

    blockch_print(blkc);

    printf("niepuste linie: %ld\n", block_linecount(blkc->blkarr));

    blockch_delete_all(blkc);
    return 0;
}


