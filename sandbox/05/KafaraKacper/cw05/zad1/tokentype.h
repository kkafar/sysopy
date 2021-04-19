#ifndef __TOKENTYPE_H__
#define __TOKENTYPE_H__

typedef 
enum TokenType
{
    COMMAND_NAME        = 0,
    INSTRUCTION_NAME    = 1,
    ARGUMENT            = 2,
    PIPE                = 3,
    NOT_CLASSIFIED      = 4
}   TokenType;


#endif