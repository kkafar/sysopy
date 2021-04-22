#include <stdlib.h>
#include <stdio.h>
#include "tokenlist.h"


CCList * cclist_create()
{
    CCList * cclist = (CCList *) malloc(sizeof(CCList));
    if (cclist_init(cclist) < 0) return NULL;
    return cclist;
}


int cclist_init(CCList * list)
{
    if (!list) return -1;
    if (( list->head = (CCListNode *) malloc(sizeof(CCListNode)) ) == NULL) return -1;
    if (( list->tail = (CCListNode *) malloc(sizeof(CCListNode)) ) == NULL) 
    {
        free(list->head);
        return -1;
    }
    list->head->next = list->tail;
    list->tail->prev = list->head;
    list->head->prev = list->tail->next = NULL;
    list->head->command_chain = list->tail->command_chain = NULL;
    list->size = 0;
    return 0;
}


void cclist_delete(CCList * list)
{
    if (!list) return;
    CCListNode * iter = list->head;
    CCListNode * tracker;

    while (iter != list->tail)
    {
        tracker = iter;
        iter = iter->next;
        cmdch_delete(tracker->command_chain);
        free(tracker);
    }
    cmdch_delete(iter->command_chain);
    free(iter);
    free(list);
}


void cclist_push_back(CCList * list, CommandChain * command_chain)
{
    if (!command_chain || !list) return;
    CCListNode * new_node = (CCListNode *) malloc(sizeof(CCListNode));
    new_node->command_chain = command_chain;
    new_node->next = list->tail;
    new_node->prev = list->tail->prev;
    list->tail->prev->next = new_node;
    list->tail->prev = new_node;
    ++(list->size);
}


void cclist_print(CCList * list)
{
    if (!list) return;
    CCListNode * iter = list->head->next;
    while (iter != list->tail)
    {
        cmdch_print(iter->command_chain);
        iter = iter->next;
    }
}