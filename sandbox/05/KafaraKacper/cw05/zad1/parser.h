#ifndef __PARSER_H__
#define __PARSER_H__

#include <stdlib.h>
#include "token.h"
#include "tokenlist.h"



#define BUFSIZE 255
#define MAX_LINE_LEN 4096

typedef enum ParseMode 
{
    INSTRUCTION = 0,
    EXECUTABLE = 1
}   ParseMode;
typedef struct FileContent
{
    CCList * command_list;
    CCList * exec_list;
}   FileContent;


FileContent * fc_create();
void fc_delete(FileContent * file_content);
void fc_print(FileContent * file_content);
CommandChain * parse_instruction(const char line[]);
CommandChain * parse_exec_line(const char line[]);
FileContent * parse_file(const char filepath[]);
char * strncpy_nulled(char * dest, const char * src, size_t n);
int get_instruction_name(const char line[], int token_start, int line_length, char * buf);
int get_next_token(const char line[], int toekn_start, int line_length, char * buf);
int get_command(Command * command, const char line[], int token_start, int line_length);
int get_argument(const char line[], int token_start, int line_length, char buf[]);
int skip_whitespace(const char line[], int start, int line_length);
size_t remove_trailing_newline(char str[]);


#endif