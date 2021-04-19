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


void cmd_delete(Command * command) 
{
    if (!command) return;

    for (int i = 0; i < command->arg_count; ++i) free(command->args[i]);

    free(command->args);
    free(command->cmd);
    free(command);
    command = NULL;
}