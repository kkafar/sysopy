#include "token.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


Command * cmd_create(const char cmd[], int argc, char * args[])
{
    if (!cmd || argc < 0 || !args) return NULL;

    int cmd_len = strlen(cmd);
    Command * command = (Command *) calloc(1, sizeof(Command));

    if (!command) return NULL;

    /* additional byte for null character */
    command->cmd = (char *) calloc(cmd_len + 1, sizeof(char)); 

    if (!command->cmd) 
    {
        free(command);
        return NULL;
    }

    strcpy(command->cmd, cmd);
    command->arg_count = argc;
    /* 0-th arg is a cmdname, argc+1 is a NULL */
    command->args = (char **) calloc(argc + 2, sizeof(char *));

    if (!command->args)
    {
        free(command->cmd);
        free(command);
        return NULL;
    }

    int arg_len;
    for (int i = 0; i < argc; ++i) 
    {
        arg_len = strlen(args[i]);
        command->args[i + 1] = (char *) calloc(arg_len + 1, sizeof(char));
        strcpy(command->args[i + 1], args[i]);
    }
    command->args[0] = (char *) calloc(cmd_len + 1, sizeof(char)); 
    strcpy(command->args[0], cmd);

    return command;
}

int cmd_init(Command * command, const char cmdname[], int argc, char * args[])
{
    if (!command || argc < 0 || !cmdname) return -2;

    int cmd_len = strlen(cmdname);

    /* additional byte for null character */
    command->cmd = (char *) calloc(cmd_len + 1, sizeof(char)); 

    if (!command->cmd) 
    {
        free(command);
        return -1;
    }

    strcpy(command->cmd, cmdname);
    command->arg_count = argc;

    if (args && argc > 0)   
    {
        command->args = (char **) calloc(argc + 2, sizeof(char *));
        if (!command->args)
        {
            free(command->cmd);
            free(command);
            return -1;
        }
        int arg_len;
        command->args[0] = (char *) calloc(cmd_len + 1, sizeof(char));
        strcpy(command->args[0], cmdname);
        for (int i = 0; i < argc; ++i) 
        {
            arg_len = strlen(args[i]);
            command->args[i + 1] = (char *) calloc(arg_len + 1, sizeof(char));
            strcpy(command->args[i + 1], args[i]);
        }
    }
    else    
        command->args = NULL;

    return 0;
}



void cmd_delete(Command * command) 
{
    if (!command) return;

    for (int i = 0; i < command->arg_count; ++i) free(command->args[i]);

    free(command->args);
    free(command->cmd);
    free(command);
    command = NULL;
}


void cmd_clean(Command * command)
{
    if (!command) return;
    
    for (int i = 0; i < command->arg_count; ++i) free(command->args[i]);

    free(command->args);
    free(command->cmd);
}


CommandChain * cmdch_create(const char name[], int command_count)
{
    if (!name || command_count <= 0) return NULL;

    int name_len = strlen(name);

    CommandChain * cmdch;

    if (( cmdch = (CommandChain *) calloc(1, sizeof(CommandChain)) ) == NULL) 
        return NULL;

    cmdch->command_count = command_count;
    cmdch->guard = 0;
    
    /* +1 byte for null character */
    if (( cmdch->name = (char *) calloc(name_len + 1, sizeof(char)) ) == NULL)
    {
        free(cmdch);
        return NULL;
    }

    strcpy(cmdch->name, name);   

    if (( cmdch->commands = (Command *) calloc(command_count, sizeof(Command)) ) == NULL)
    {
        free(cmdch->name);
        free(cmdch);
        return NULL;
    }

    return cmdch;
}


void cmdch_delete(CommandChain * command_chain)
{
    if (!command_chain) return;

    free(command_chain->name);

    for (int i = 0; i < command_chain->command_count; ++i) 
        cmd_clean(command_chain->commands + i);

    free(command_chain->commands);
    free(command_chain);
    command_chain = NULL;
}


void cmd_print(Command * command)
{
    printf("cmd name(%d): %s ", command->arg_count,command->cmd);
    if (!command->args) 
    {
        printf("no args\n");
        return;
    }
    for (int i = 1; i <= command->arg_count; ++i) 
        printf("%s(%ld) ", command->args[i], strlen(command->args[i]));
    printf("\n");
}

void cmdch_print(CommandChain * command_chain) 
{
    printf("ins. name: %s, commands: %d\n", command_chain->name, command_chain->command_count);
    for (int i = 0; i < command_chain->command_count; ++i) 
        cmd_print(command_chain->commands + i);
}