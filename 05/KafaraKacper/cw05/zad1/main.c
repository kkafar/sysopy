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
#include <unistd.h>
#include <wait.h>
#include "token.h"
#include "parser.h"
#include "util.h"
#include "tokenlist.h"


size_t remove_trailing_newline(char str[]);
CommandChain * find_instruction(FileContent * file_content, char * instruction);
size_t calculate_exec_size(CCList * exec_list);
int * execute_command_chain(CommandChain * command_chain, int input_fd, int ouput_fd);


int main(int argc, char * argv[]) 
{
    if (argc != 2) err("bad arg count", __FILE__, __func__, __LINE__);

    FileContent * file_content;
    if ((file_content = parse_file(argv[1])) == NULL) err("failed to parse file", __FILE__, __func__, __LINE__);

    CommandChain * instruction;
    CCListNode * iterator = file_content->exec_list->head->next;
    int command_count;
    while (iterator != file_content->exec_list->tail)
    {
        command_count = iterator->command_chain->command_count;
        pid_t pipes[command_count - 1][2];
        for (int i = 0; i < command_count - 1; ++i)
            if (pipe(pipes[i]) < 0) syserr("pipe", __FILE__, __func__, __LINE__);

        for (int i = 0; i < command_count; ++i)
        {
            if ((instruction = find_instruction(file_content, iterator->command_chain->commands[i].cmd)) == NULL)
                err("invalid instruction", __FILE__, __func__, __LINE__);
                
            printf("Executing: %s\n", instruction->name);

            if (command_count == 1)
                execute_command_chain(instruction, -1, -1);
            else if (i == 0)
                execute_command_chain(instruction, -1, pipes[0][1]);
            else if (i < command_count - 1)
                execute_command_chain(instruction, pipes[i - 1][0], pipes[i][1]);
            else 
                execute_command_chain(instruction, pipes[i - 1][0], -1);
        }
        
        for (int i = 0; i < command_count - 1; ++i)
        {
            close(pipes[i][0]);
            close(pipes[i][1]);
        }
        while (waitpid(-1, NULL, 0) != -1);
        iterator = iterator->next;
    }
    fc_delete(file_content); 
    exit(EXIT_SUCCESS);
}


CommandChain * find_instruction(FileContent * file_content, char * instruction)
{
    if (!file_content || !file_content->command_list) return NULL;

    CCListNode * iter = file_content->command_list->head->next;
    while (iter != file_content->command_list->tail)
    {
        if (strcmp(iter->command_chain->name, instruction) == 0) 
            return iter->command_chain;
        iter = iter->next;
    }
    return NULL;
}

size_t calculate_exec_size(CCList * exec_list)
{
    if (!exec_list) return -1;

    size_t size = 0;
    CCListNode * iter = exec_list->head->next;
    while (iter != exec_list->tail)
    {
        size += iter->command_chain->command_count;
        iter = iter->next;
    }
    return size;
}


int * execute_command_chain(CommandChain * command_chain, int input_fd, int output_fd)
{
    if (!command_chain) return NULL;

    int pipes[command_chain->command_count - 1][2];

    for (int i = 0; i < command_chain->command_count - 1; ++i)
        if (pipe(pipes[i]) < 0) syserr("pipe", __FILE__, __func__, __LINE__);

    pid_t cpid;
    for (int i = 0; i < command_chain->command_count; ++i)
    {
        if (( cpid = fork() ) < 0) syserr("fork", __FILE__, __func__, __LINE__);
        else if (cpid == 0)
        {
            if (command_chain->command_count == 1)
            {
                if (input_fd >= 0)
                    if (dup2(input_fd, STDIN_FILENO) < 0)       syserr("dup2", __FILE__, __func__, __LINE__);
                if (output_fd >= 0)
                    if (dup2(output_fd, STDOUT_FILENO) < 0)     syserr("dup2", __FILE__, __func__, __LINE__);
            }
            else if (i == 0) /* first command */
            {
                if (input_fd >= 0)
                    if (dup2(input_fd, STDIN_FILENO) < 0)       syserr("dup2", __FILE__, __func__, __LINE__);
                if (dup2(pipes[0][1], STDOUT_FILENO) < 0)       syserr("dup2", __FILE__, __func__, __LINE__);
            }
            else if (i < command_chain->command_count - 1)
            {
                if (dup2(pipes[i - 1][0], STDIN_FILENO) < 0)    syserr("dup2", __FILE__, __func__, __LINE__);
                if (dup2(pipes[i][1], STDOUT_FILENO) < 0)       syserr("dup2", __FILE__, __func__, __LINE__);
            }
            else /* last command */
            {
                if (output_fd >= 0)
                    if (dup2(output_fd, STDOUT_FILENO) < 0)     syserr("dup2", __FILE__, __func__, __LINE__);
                if (dup2(pipes[i - 1][0], STDIN_FILENO) < 0)    syserr("dup2", __FILE__, __func__, __LINE__);
            }
            for (int i = 0; i < command_chain->command_count - 1; ++i)
            {
                close(pipes[i][0]);
                close(pipes[i][1]);
            }
            execvp(command_chain->commands[i].cmd, command_chain->commands[i].args);
            syserr("execvp", __FILE__, __func__, __LINE__);
        }
    }
    close(input_fd); close(output_fd);
    for (int i = 0; i < command_chain->command_count - 1; ++i)
    {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
    return NULL;
}