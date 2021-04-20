/** 
 * Assumptions:
 * 
 * All tokens are separated by single space.
 * There is no empty definition.
 * All lines are ended with newline character
 */


#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include "token.h"
#include "parser.h"
#include "util.h"

#define MAX_LINE_LEN 4096


void syserr(const char errmsg[], const char file[], int line);
void err(const char errmsg[], const char file[], int line);
size_t remove_trailing_newline(char str[]);


int main(int argc, char * argv[]) 
{
    if (argc != 2) err("bad arg count", __FILE__, __LINE__);

    FILE * commands_file;

    if (( commands_file = fopen(argv[1], "r") ) == NULL) syserr(NULL, __FILE__, __LINE__);

    char buf[MAX_LINE_LEN];

    fgets(buf, MAX_LINE_LEN - 1, commands_file);

    CommandChain * command_chain = parse_instruction(buf);

    cmdch_print(command_chain);

    cmdch_delete(command_chain);

    fclose(commands_file);


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

