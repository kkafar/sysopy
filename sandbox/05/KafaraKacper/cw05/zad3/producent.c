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
#define CHAR_NUM    5


int main(int argc, char * argv[])
{
    if (argc != 5) err("bad arg count", __FILE__, __func__, __LINE__);

    srand(time(NULL));

    int fd;
    FILE * file_fifo;
    if ((fd = mkfifo(argv[FIFO_PATH], O_WRONLY)) < 0)   syserr("mkfifo", __FILE__, __func__, __LINE__);
    if ((file_fifo = fdopen(fd, "w")) == NULL)          syserr("fdopen", __FILE__, __func__, __LINE__);

    FILE * file;
    if ((file = fopen(argv[FILE_PATH], "r")) == NULL)   syserr("fopen", __FILE__, __func__, __LINE__);

    long N = strtol(argv[CHAR_NUM], NULL, 10);
    if (N <= 0) err("invalid N value", __FILE__, __func__, __LINE__);
    long row_num_width = strlen(argv[ROW_NUM]);
    char buf[N + 2], buf2[N + 2 + row_num_width];

    int bytes_read;
    while ( (bytes_read = fread(buf, sizeof(char), N, file)) > 0)
    {   
        buf[bytes_read] = 0;
        strcpy(buf2, argv[ROW_NUM]);
        buf2[row_num_width] = ' ';
        buf2[row_num_width + 1] = 0;
        strcat(buf2, buf);
        sleep(rand() % 3 + 1);

        if (fwrite(buf2, sizeof(char), bytes_read, file_fifo) <= 0) err("fwrite", __FILE__, __func__, __LINE__);
    }
    if (ferror(file)) err("fread", __FILE__, __func__, __LINE__);

    fclose(file);
    fclose(file_fifo);
    exit(EXIT_SUCCESS);
}