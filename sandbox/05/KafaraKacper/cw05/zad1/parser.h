#ifndef __PARSER_H__
#define __PARSER_H__

#include "token.h"
#include "tokenlist.h"
#include <stdlib.h>


#define BUFSIZE 255
#define MAX_LINE_LEN 4096


CommandChain * parse_instruction(const char line[]);
CCList * parse_file(const char filepath[]);
char * strncpy_nulled(char * dest, const char * src, size_t n);
int get_instruction_name(const char line[], int token_start, int line_length, char * buf);
int get_command(Command * command, const char line[], int token_start, int line_length);
int get_argument(const char line[], int token_start, int line_length, char buf[]);
int skip_whitespace(const char line[], int start, int line_length);
size_t remove_trailing_newline(char str[]);

typedef enum ParseMode 
{
    INSTRUCTION = 0,
    EXECUTABLE = 1
}   ParseMode;

typedef struct FileStructure
{
    int n_instructions;
    int n_executables;
}   FileStructure;


#endif