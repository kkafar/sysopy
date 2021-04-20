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
#include "tokenlist.h"



size_t remove_trailing_newline(char str[]);


int main(int argc, char * argv[]) 
{
    if (argc != 2) err("bad arg count", __FILE__, __func__, __LINE__);

    FILE * commands_file;

    if (( commands_file = fopen(argv[1], "r") ) == NULL) syserr(NULL, __FILE__, __func__, __LINE__);

    char buf[MAX_LINE_LEN];

    fgets(buf, MAX_LINE_LEN - 1, commands_file);

    CommandChain * command_chain = parse_instruction(buf);

    // cmdch_print(command_chain);

    CCList * list = cclist_create();
    cclist_push_back(list, command_chain);

    // cmdch_delete(command_chain);
    cclist_print(list);
    cclist_delete(list);
    
    fclose(commands_file);


    exit(EXIT_SUCCESS);
}
