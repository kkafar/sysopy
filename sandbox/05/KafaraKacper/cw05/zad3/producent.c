#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <fcntl.h>
#include <time.h>
#include "util.h"

#define FIFO_PATH   1
#define ROW_NUM     2
#define FILE_PATH   3
#define CHAR_NUM    4


int main(int argc, char * argv[])
{
    if (argc != 5) err("bad arg count; usage: ./producent fifo_path row_num file_path char_num", __FILE__, __func__, __LINE__);

    srand(time(NULL));

    FILE * file_fifo;
    if ((file_fifo = fopen(argv[FIFO_PATH], "w")) == NULL) syserr("fopen", __FILE__, __func__, __LINE__);

    FILE * file;
    if ((file = fopen(argv[FILE_PATH], "r")) == NULL)   syserr("fopen", __FILE__, __func__, __LINE__);

    long N = strtol(argv[CHAR_NUM], NULL, 10);
    if (N <= 0) err("invalid N value", __FILE__, __func__, __LINE__);

    long row_num_width = strlen(argv[ROW_NUM]);

    char buf[N + 3], buf2[N + 2 + row_num_width];
    buf[N] = 0;

    int bytes_read;
    while ( (bytes_read = fread(buf, sizeof(char), N, file) ) > 0)
    {   
        printf("%s:id(%s): read from input file: %s\n", __FILE__, argv[ROW_NUM], buf);
        if (bytes_read < N)
            for (int i = bytes_read; i < N; ++i) buf[i] = ' ';
        
        buf[N] = 0;
        strcpy(buf2, argv[ROW_NUM]);
        buf2[row_num_width] = ' ';
        buf2[row_num_width + 1] = 0;
        strcat(buf2, buf);

        sleep(rand() % 2 + 1);      /* sleep for one or two seconds */
        // fwrite(buf2, sizeof(char), strlen(buf2), stdout); printf(" -- %ld bytes\n", strlen(buf2));
        if (fwrite(buf2, sizeof(char), strlen(buf2), file_fifo) <= 0) err("fwrite", __FILE__, __func__, __LINE__);
    }
    if (ferror(file)) err("fread", __FILE__, __func__, __LINE__);

    fclose(file);
    fclose(file_fifo);
    exit(EXIT_SUCCESS);
}