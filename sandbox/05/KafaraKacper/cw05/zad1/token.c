#include "token.h"
#include <string.h>
#include <stdlib.h>


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
    command->args = (char **) calloc(argc, sizeof(char *));

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
        command->args[i] = (char *) calloc(arg_len + 1, sizeof(char));
        strcpy(command->args[i], args[i]);
    }

    return command;
}

int cmd_init(Command * command, const char cmdname[], int argc, char * args[])
{
    if (!command || argc < 0 || !args || !cmdname) return -2;

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
    command->args = (char **) calloc(argc, sizeof(char *));

    if (!command->args)
    {
        free(command->cmd);
        free(command);
        return -1;
    }

    int arg_len;
    for (int i = 0; i < argc; ++i) 
    {
        arg_len = strlen(args[i]);
        command->args[i] = (char *) calloc(arg_len + 1, sizeof(char));
        strcpy(command->args[i], args[i]);
    }

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