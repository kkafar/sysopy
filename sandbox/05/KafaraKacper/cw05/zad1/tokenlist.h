#ifndef __TOKEN_LIST_H__
#define __TOKEN_LIST_H__

#include "token.h"

typedef struct CommandChainListNode 
{
    CommandChain * command_chain;
    struct CommandChainListNode * prev, * next;
}   CCListNode;

typedef struct CommandChainList
{
    CCListNode * head, * tail;
}   CCList;

CCList * cclist_create();
int cclist_init(CCList * list);
void cclist_delete(CCList * list);
void cclist_push_back(CCList * list, CommandChain * command_chain);
void cclist_print(CCList * list);
#endif