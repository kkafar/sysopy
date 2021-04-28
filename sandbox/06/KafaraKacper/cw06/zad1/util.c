#include "util.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>


void syserr(const char errmsg[], const char file[], const char func[], int line)
{
    int errnum = errno;
    if (!errmsg)            errmsg = "";
    if (!file)              file = "";
    if (!func)              func = "";
    if (line < 0)           line = -1;
    fprintf(stderr, "%s:%s:%d: %s. %s\n", file, func, line, strerror(errnum), errmsg);
    exit(errnum);
}


void err(const char errmsg[], const char file[], const char func[], int line) 
{
    if (!errmsg)            errmsg = "empty error message";
    if (!file)              file = "";
    if (!func)              func = "";
    if (line < 0)           line = -1;
    fprintf(stderr, "%s:%s:%d: %s\n", file, func, line, errmsg);
    exit(EXIT_FAILURE);
}

void syserr_noexit(const char errmsg[], const char file[], const char func[], int line)
{
    int errnum = errno;
    if (!errmsg)            errmsg = "";
    if (!file)              file = "";
    if (!func)              func = "";
    if (line < 0)           line = -1;
    fprintf(stderr, "%s:%s:%d: %s. %s\n", file, func, line, strerror(errnum), errmsg);
}


void err_noexit(const char errmsg[], const char file[], const char func[], int line) 
{
    if (!errmsg)            errmsg = "empty error message";
    if (!file)              file = "";
    if (!func)              func = "";
    if (line < 0)           line = -1;
    fprintf(stderr, "%s:%s:%d: %s\n", file, func, line, errmsg);
}



int clearbuf(char buf[], size_t size)
{
    if (!buf || size <= 0) return -1;
    for (int i = 0; i < size; ++i) buf[i] = 0;
    return 0;
}