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
#define FILE_PATH   2
#define CHAR_NUM    3



int main(int argc, char * argv[])
{    
    if (argc != 4) err("bad arg count", __FILE__, __func__, __LINE__);

    int fd;
    FILE * file_fifo;
    if ((fd = mkfifo(argv[FIFO_PATH], O_RDONLY)) < 0)   syserr("mkfifo", __FILE__, __func__, __LINE__);
    if ((file_fifo = fdopen(fd, "r")) == NULL)          syserr("fdopen", __FILE__, __func__, __LINE__);

    FILE * file;
    if ((file = fopen(argv[FILE_PATH], "w")) == NULL)   syserr("fopen", __FILE__, __func__, __LINE__);

    fclose(file);
    fclose(file_fifo);
    exit(EXIT_SUCCESS);
}