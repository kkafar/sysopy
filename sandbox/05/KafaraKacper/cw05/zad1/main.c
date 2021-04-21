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
// CommandChain


int main(int argc, char * argv[]) 
{
    if (argc != 2) err("bad arg count", __FILE__, __func__, __LINE__);

    FileContent * file_content;
    if ((file_content = parse_file(argv[1])) == NULL) err("Failed to parse file", __FILE__, __func__, __LINE__);

    

    fc_print(file_content);
    fc_delete(file_content); 
    exit(EXIT_SUCCESS);
}

