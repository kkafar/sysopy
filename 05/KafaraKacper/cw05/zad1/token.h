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
    char * name;
    int command_count; 
    int guard;
    Command * commands;
}   CommandChain;


Command * cmd_create(const char cmd[], int argc, char * args[]);

int cmd_init(Command * command, const char cmdname[], int argc, char * args[]);

void cmd_clean(Command * command);

void cmd_delete(Command * command);

CommandChain * cmdch_create(const char name[], int command_count);

// void cmdch_insert()

void cmdch_delete(CommandChain * cmdch);

void cmd_print(Command * command);

void cmdch_print(CommandChain * command_chain);

#endif