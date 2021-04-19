#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>


#define MAX_LINE_LEN 4096


void syserr(const char errmsg[], const char file[], int line);
void err(const char errmsg[], const char file[], int line);
size_t remove_trailing_newline(char str[]);


int main(int argc, char * argv[]) 
{
    if (argc != 2) err("bad arg count", __FILE__, __LINE__);

    FILE * commands_file;

    if (( commands_file = fopen(argv[1], "r") ) == NULL) syserr(NULL, __FILE__, __LINE__);


    exit(EXIT_SUCCESS);
}




size_t remove_trailing_newline(char str[]) 
{
    if (!str) return -1;

    size_t str_length = strlen(str);
    if (str[str_length - 1] == '\n') 
    {
        str[str_length - 1] = 0;
        return str_length - 1;
    }
    return str_length;
}


void syserr(const char errmsg[], const char file[], int line)
{
    int errnum = errno;
    if (!errmsg)            errmsg = "";
    if (!file)              file = "";
    if (line < 0)           line = -1;
    fprintf(stderr, "%s: %d: %s. %s\n", file, line, strerror(errnum), errmsg);
    exit(errnum);
}


void err(const char errmsg[], const char file[], int line) 
{
    if (!errmsg)            errmsg = "empty error message";
    if (!file)              file = "";
    if (line < 0)           line = -1;
    fprintf(stderr, "%s: %d: %s\n", file, line, errmsg);
    exit(EXIT_FAILURE);
}