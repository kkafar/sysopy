#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

#define MAX_CLIENTS     50
#define MAX_MSG_LEN     9200
#define MAX_QNAME_LEN   50
#define MT_CONNECT      5
#define MT_INIT         4
#define MT_LIST         3
#define MT_DISCONNECT   2
#define MT_STOP         1
#define MT_CHAT         6
#define MT_UBOUND       20

#define FTOK_ID1         15
// #define FTOK_ID2         1
// #define FTOK_ID3         2
// #define FTOK_ID4         3
// #define FTOK_ID5         4
// #define FTOK_ID6         5
// #define FTOK_ID7         6
// #define FTOK_ID8         7
// #define FTOK_ID9         8
// #define FTOK_ID10         9
// #define FTOK_ID11        10
// #define FTOK_ID12        11
// #define FTOK_ID13        12
// #define FTOK_ID14        13
// #define FTOK_ID15        14
#define FTOK_SERVER_ID   100

#define SERVER_QUEUE_NAME "/serverqueue"

typedef struct Message
{
    long type;
    char buf[MAX_MSG_LEN];
}   Message;

int set_message(Message * msg, long type, char * buf);

#endif