#include "zad1.h"


void block_init(block * blk, size_t size) 
{
    if (size <= 0 || blk == NULL) return;

    blk->fline = (char **) calloc(size, sizeof(char *));

    if (blk->fline == NULL) return;

    blk->size = size;    
}


block * block_create(size_t size)
{
    block * blk = calloc(1, sizeof(block));
    block_init(blk, size);
    return blk;
}


void block_clear(block * blk) 
{
    if (blk == NULL || blk->fline == NULL) return;

    for (size_t i = 0; i < blk->size; ++i) 
        if (*(blk->fline + i) != NULL) free(*(blk->fline + i));
    
    free(blk->fline);
    blk->fline = NULL;
    // blk = NULL;
}


void block_delete(block * blk)
{
    if (!blk) return;

    block * blkcopy = blk;
    block_clear(blkcopy);

    free(blk);
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
    blkc = NULL;
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
    for (size_t i = 0; i < blk->size; ++i)
        if (*(blk->fline + i)) printf("L%-3ld: %s\n", i, *(blk->fline + i));
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


size_t blockch_read_block(blockch * blkc, const char * pathname)
{
    if (!blkc || !pathname || !blkc->blkarr) return -1;

    size_t empty_index = -1;

    for (size_t i = 0; i < blkc->size; ++i)
    {
        if (!blkc->blkarr[i].fline) 
        {
            empty_index = i;
            break;
        }
    }

    if (empty_index == -1) return -1;

    block_read(blkc->blkarr + empty_index, pathname);

    if (!blkc->blkarr[empty_index].fline) return -1;

    return empty_index;
}


void block_read(block * blk, const char * pathname)
{
    if (!blk || !pathname) return;

    FILE * file = fopen(pathname, "r");

    if (!file) return;

    size_t lcount = file_line_count(pathname);

    block_init(blk, lcount);

    char buf[LINE_WIDTH];

    for (size_t i = 0; i < blk->size; ++i)
    {
        if (fgets(buf, LINE_WIDTH, file) == NULL) break;
        block_insert_at(blk, i, buf);
    }

    fclose(file);
}


void merge_files(block * fseq, blockch * blkc, int save_flag, block * savefile)
{
    if (!fseq || !blkc || 2 * blkc->size != fseq->size) return;

    FILE * f1, * f2;
    char buf1[LINE_WIDTH], buf2[LINE_WIDTH];
    int flag = 0, idx;
    size_t leng;
    char * pathname1, * pathname2, * catpathname;
    block * blk;
    
    for (size_t i = 0; i < fseq->size - 1; i += 2) 
    {
        pathname1 = *(fseq->fline + i);
        pathname2 = *(fseq->fline + i + 1);
        
        f1 = fopen(pathname1, "r");
        f2 = fopen(pathname2, "r");

        if (!f1 || !f2) 
        {
            fprintf(stderr, "%s: %d: Could not open one of files: %s, %s\n", __func__, __LINE__, pathname1, pathname2);
            continue;
        }

        leng = file_line_count(pathname1) + file_line_count(pathname2);

        blk = blkc->blkarr + (i >> 1);
        block_init(blk, leng);

        flag = idx = 0;
        while (!flag)
        {
            if (fgets(buf1, LINE_WIDTH, f1) == NULL) flag = 1;
            else block_insert_at(blk, idx++, buf1);

            if (fgets(buf2, LINE_WIDTH, f2) == NULL) flag += 2;
            else block_insert_at(blk, idx++, buf2);
        }

        if (flag == 1)
        {
            while (fgets(buf2, LINE_WIDTH, f2) != NULL)
                block_insert_at(blk, idx++, buf2);
        }
        else if (flag == 2)
        {
            while (fgets(buf1, LINE_WIDTH, f1) != NULL)
                block_insert_at(blk, idx++, buf1);
        }

        fclose(f1);
        fclose(f2);

        if (save_flag == 1)
        {
            if (!savefile) {
                catpathname = get_tmp_pathname(pathname1, pathname2);
                block_save(blk, catpathname);
                free(catpathname);
            } else {
                block_save(blk, savefile->fline[i / 2]);
            }
        }
    }    
}


size_t file_line_count(const char * pathname) 
{
    FILE * f = fopen(pathname, "r");

    if (!f) return 0;

    char c;
    int count = 0;
    while ((c = fgetc(f)) != EOF)
        if (c == '\n') ++count;

    fclose(f);
    return count;
}


void block_save(block * blk, const char * pathname)
{
    FILE * f = fopen(pathname, "w");

    if (!f) return;

    for (size_t i = 0; i < blk->size; ++i) 
        if (*(blk->fline + i)) fputs(*(blk->fline + i), f);

    fclose(f);
}


void rm_tmp_files(block * fseq)
{
    if (!fseq) return;

    for (size_t i = 0; i < fseq->size; ++i) remove(fseq->fline[i]);
}


char * get_tmp_pathname(const char * pn1, const char * pn2)
{
    size_t sizepn1 = strlen(pn1);
    size_t sizepn2 = strlen(pn2);
    char * newpn = (char *) calloc((sizepn1 + sizepn2 + 1), sizeof(char));

    size_t i = 0;

    for (size_t j = 0; j < sizepn1; ++j) newpn[i++] = pn1[j];
    for (size_t j = 0; j < sizepn2; ++j) newpn[i++] = pn2[j];

    return newpn;
}