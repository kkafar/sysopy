#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>

struct block 
{
    char ** fline;
    size_t size;
}; typedef struct block block;

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

void block_delete(block * blk)
{
    if (!blk) return;

    block * blkcopy = blk;
    block_clear(blkcopy);

    free(blk);
}


void block_read(block * blk, const char * pathname)
{
    if (!blk || !pathname) return;

    FILE * file = fopen(pathname, "r");

    if (!file) return;

    size_t lcount = file_line_count(pathname);

    block_init(blk, lcount);

    char buf[FILENAME_MAX];

    for (size_t i = 0; i < blk->size; ++i)
    {
        fgets(buf, FILENAME_MAX, file);
        block_insert_at(blk, i, buf);
    }
    fclose(file);
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



int main(int argc, char * argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Invalid number of arguments: %d. Expectd 4.\n", argc);
        exit(1);
    }

    const char * input_pathname     = argv[1];
    const char * output_pathname    = argv[2];
    const char * str1               = argv[3];
    const char * str2               = argv[4];

    size_t str1_len = strlen(str1);
    size_t str2_len = strlen(str2);
    
    char * buf = (char *) calloc(str1_len, sizeof(char));
    char ch;
    bool matched = false;

    FILE * input_file = fopen(input_pathname, "r");
    FILE * output_file = fopen(output_pathname, "r+");

    if (!input_file || !output_file) {
        int errnum = errno;
        fprintf(stderr, "%d, errno: %d, %s\n", __LINE__, errnum, strerror(errnum));
        if (input_file) fclose(input_file);
        if (output_file) fclose(output_file);
        exit(errnum);
    }

    size_t nlines = file_line_count(input_file);


    
    fclose(input_file);
    fclose(output_file);

    return 0;
}