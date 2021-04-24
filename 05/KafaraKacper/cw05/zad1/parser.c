#include "parser.h"
#include "util.h"
#include <string.h>
#include <stdio.h> 


CommandChain * parse_instruction(const char line[])
{
    if (!line) return NULL;
    printf("%s\n", line);

    char buf[BUFSIZE];        
    int command_count = 1;
    int line_length = strlen(line);

    for (int i = 0; i < line_length; ++i)
        if (line[i] == '|') ++command_count;

    int token_end = get_instruction_name(line, 0, line_length, buf);
    int token_start = skip_whitespace(line, token_end, line_length);

    CommandChain * command_chain = cmdch_create(buf, command_count);

    for (int i = 0; i < command_count; ++i) 
    {
        token_end = get_command(command_chain->commands + i, line, token_start, line_length);
        token_start = skip_whitespace(line, token_end, line_length);
    }
    return command_chain;
}


CommandChain * parse_exec_line(const char line[])
{
    if (!line) return NULL;
    char buf[BUFSIZ];
    int exec_count = 1;
    int line_length = strlen(line);
    for (int i = 0; i < line_length; ++i) 
        if (line[i] == '|') ++exec_count;

    int token_end, token_start = 0;
    CommandChain * command_chain = cmdch_create("EXEC", exec_count);
    
    for (int i = 0; i < exec_count; ++i)
    {
        token_end = get_next_token(line, token_start, line_length, buf);
        token_start = skip_whitespace(line, token_end, line_length);
        cmd_init(command_chain->commands + i, buf, 0, NULL);
    }
    return command_chain;
}

int get_next_token(const char line[], int token_start, int line_length, char * buf)
{
    if (!line || token_start < 0 || token_start >= line_length)
    {
        err_noexit("invalid arguments", __FILE__, __func__, __LINE__);
        return -1;
    }

    int i = token_start, token_end = -1, bufi = -1;
    while (i < line_length && (line[i] == ' ' || line[i] == '|' || line[i] == '='))
        ++i;

    if (i >= line_length) 
    {
        err_noexit("invalid call", __FILE__, __func__, __LINE__);
        return -1;
    }

    while (i < line_length)
    {
        if (line[i] == ' ')
        {
            token_end = i - 1;                
            break;
        }
        buf[++bufi] = line[i++];
    }
    if (i == line_length) token_end = i - 1;
    buf[bufi + 1] = 0;
    return token_end;
}


int get_instruction_name(const char line[], int token_start, int line_length, char * buf)
{
    if (!line || token_start < 0 || token_start >= line_length || !buf)
    {
        err_noexit("invalid arguments", __FILE__, __func__, __LINE__);
        return -1;
    }

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
    {
        err_noexit("invalid arguments", __FILE__, __func__, __LINE__);
        return -1;
    }

    int arg_count = -1, i = token_start;

    while (i < line_length && line[i] != '|')
    {
        if (line[i] == ' ') ++arg_count;
        ++i;
    }
    if (i == line_length) ++arg_count;

    char buf[BUFSIZE];

    /* collecting command name */
    int token_end;
    
    if (( token_end = get_argument(line, token_start, line_length, buf) ) < 0) return -1;


    /* additional byte for null */
    char command_name[token_end - token_start + 2];
    strcpy(command_name, buf);
    token_start = skip_whitespace(line, token_end, line_length);

    char * arguments[arg_count];

    for (int i = 0; i < arg_count; ++i)
    {
        arguments[i] = (char *) malloc(sizeof(char) * BUFSIZE);
        if (( token_end = get_argument(line, token_start, line_length, arguments[i]) ) < 0)
            return -1;

        token_start = skip_whitespace(line, token_end, line_length);
    }

    if (cmd_init(command, command_name, arg_count, arguments) < 0) 
    {   
        for (int i = 0; i < arg_count; ++i) free(arguments[i]);
        return -2;
    }

    for (int i = 0; i < arg_count; ++i) free(arguments[i]);
    
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
    
    if (i == line_length) token_end = i - 1;

    if (token_end < 0) return -1;

    buf[bufi + 1] = 0;
    return token_end;
}


int skip_whitespace(const char line[], int start, int line_length) 
{
    ++start;
    while (start < line_length && ((line[start] == ' ') || line[start] == '=' || line[start] == '|')) 
        ++start;

    if (start >= line_length) 
        return -1;

    return start;
}

FileContent * parse_file(const char filepath[])
{
    if (!filepath) return NULL;

    FILE * file;
    if (( file = fopen(filepath, "r")) == NULL) return NULL;

    char buf[MAX_LINE_LEN];
    ParseMode parse_mode = INSTRUCTION;

    FileContent * file_content = (FileContent *) malloc(sizeof(FileContent));
    if ((file_content->command_list = cclist_create()) == NULL) 
    {
        free(file_content);
        fclose(file);
        return NULL;
    }
    if ((file_content->exec_list = cclist_create()) == NULL)
    {
        cclist_delete(file_content->command_list);
        free(file_content);
        fclose(file);
        return NULL;
    }


    while (fgets(buf, MAX_LINE_LEN - 1, file) != NULL)
    {
        if (buf[0] == '\n')
        {
            parse_mode = EXECUTABLE;
            continue;
        }

        if (parse_mode == INSTRUCTION)
        {
            remove_trailing_newline(buf);
            cclist_push_back(file_content->command_list, parse_instruction(buf));
        }
        else
        {
            remove_trailing_newline(buf);
            cclist_push_back(file_content->exec_list, parse_exec_line(buf));
        }

    }
    fclose(file);
    return file_content;
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


FileContent * fc_create()
{
    FileContent * file_content;
    if ((file_content = (FileContent *) malloc(sizeof(FileContent))) == NULL) return NULL;
    if ((file_content->command_list = cclist_create()) == NULL) 
    {
        free(file_content);
        return NULL;
    }
    if ((file_content->exec_list = cclist_create()) == NULL)
    {
        cclist_delete(file_content->command_list);
        free(file_content);
        return NULL;
    }
    return file_content;
}


void fc_delete(FileContent * file_content)
{
    if (!file_content) return;
    cclist_delete(file_content->command_list);
    cclist_delete(file_content->exec_list);
    free(file_content);    
}

void fc_print(FileContent * file_content)
{
    if (!file_content) return;
    cclist_print(file_content->command_list);
    cclist_print(file_content->exec_list);
}

