#ifndef __TOKEN_H__
#define __TOKEN_H__

#include "tokentype.h"

typedef
struct Command 
{
    char * cmd;
    int arg_count;
    char ** args;
}   Command;

typedef
struct CommandChain
{
    int command_count; 
    Command * commands;
}   CommandChain;


Command * cmd_create(const char cmd[], int argc, char * args[]);

void cmd_delete(Command * command);


// CommandChain * cmdch_create(const)




#endif __TOKEN_H__