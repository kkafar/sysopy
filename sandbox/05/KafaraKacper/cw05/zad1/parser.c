#include "parser.h"
#include <string.h>


void parse_instruction(const char line[])
{
    if (!line) return;


    char buf[BUFSIZE];        
    int bufidx = -1, command_count = 1;
    int line_length = strlen(line);
    TokenType token_type = NOT_CLASSIFIED;

    for (int i = 0; i < line_length; ++i)
        if (line[i] == '|') ++command_count;

    


}


char * strncpy_nulled(char * dest, const char * src, size_t n)
{
    strncpy(dest, src, n);
    dest[n] = 0;
    return dest;
}


int get_instruction_name(const char line[], int token_start, int line_length, char * buf)
{
    if (!line || token_start < 0 || token_start >= line_length || !buf)
        return -1;

    int i = token_start, token_end = -1;
    while (i < line_length) 
    {
        if (line[i] == '=')
        {
            token_end = i - 2;
            break;
        }
        ++i;
    } 

    if (token_end < 0) return -1;

    for (int i = token_start, bufi = 0; i <= token_end; ++i, ++bufi) 
        buf[bufi] = line[i];

    buf[token_end - token_start + 1] = 0;
    return token_end;
}


int get_command(Command * command, const char line[], int token_start, int line_length)
{
    if (!line || token_start < 0 || token_start >= line_length || !command)
        return -1;

    int arg_count = -1, i = token_start;

    while (i < line_length && line[i] != '|')
    {
        if (line[i] == ' ') ++arg_count;
    }

    char buf[BUFSIZE];

    /* collecting command name */
    int token_end;
    
    if (( token_end = get_argument(line, token_start, line_length, buf) ) < 0) return -1;

    /* additional byte for null */
    char command_name[token_end - token_start + 2];
    strcpy(command_name, buf);
    token_start = token_end + 2;

    char arguments[arg_count][BUFSIZE];
    for (int i = 0; i < arg_count; ++i)
    {
        if (( token_end = get_argument(line, token_start, line_length, arguments[arg_count]) ) < 0)
            return -1;
    }

    if (cmd_init(command, command_name, arg_count, arguments) < 0) return -2;

    return token_end;
}


int get_argument(const char line[], int token_start, int line_length, char buf[])
{
    if (!line || token_start < 0 || token_start >= line_length)
        return -1;

    int i = token_start, bufi = -1, token_end = -1;

    while (i < line_length)
    {
        if (line[i] == ' ')
        {
            token_end = i - 1;
            break;
        }
        buf[++bufi] = line[i++];
    }
    
    if (token_end < 0) return -1;

    buf[bufi + 1] = 0;
    return token_end;
}