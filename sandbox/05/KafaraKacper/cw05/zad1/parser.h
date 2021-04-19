#ifndef __PARSER_H__
#define __PARSER_H__

#include "token.h"
#include "tokentype.h"
#include <stdlib.h>

#define BUFSIZE 255


void parse_instruction(const char line[]);
char * strncpy_nulled(char * dest, const char * src, size_t n);
int get_instruction_name(const char line[], int token_start, int line_length, char * buf);
int get_command(Command * command, const char line[], int token_start, int line_length);
int get_argument(const char line[], int token_start, int line_length, char buf[]);



#endif