#ifndef __PARSER_H__
#define __PARSER_H__

#include <stdlib.h>
#include "token.h"
#include "tokenlist.h"



#define BUFSIZE 255     /* max length of singular command argument */
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


/**
 * @brief allocate memory for FileContent structure
 * @return pointer to newly object
 */
FileContent * fc_create();

/**
 * @brief free memory allocated for FileContent structure
 * @param file_content - pointer to to-be-freed object
 */
void fc_delete(FileContent * file_content);

void fc_print(FileContent * file_content);

/**
 * @brief parse single instruction
 * @param line text defining single instruction 
 * @return pointer to command chain representing instruction
 */
CommandChain * parse_instruction(const char line[]);

/**
 * @brief parse single to-be-executed command
 * @param line test defining commands to be executed`
 * @return pointer to command chain representing to-be-executed commands
 */
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