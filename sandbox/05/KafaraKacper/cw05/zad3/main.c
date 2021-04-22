#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include "util.h"


int main(int argc, char * argv[])
{
    printf("hello\n");
    printf("%d\n", PIPE_BUF);
    exit(EXIT_SUCCESS);
}