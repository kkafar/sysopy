#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <sys/select.h>
#include "util.h"
#include "constants.h"


int SRVR_PUB_Q_ID = -1;
int SRVR_FTOK_KEY = -1;
key_t SRVR_Q_KEY  = -1;


typedef struct Client 
{
    long queue_id; 
    bool busy;
}   Client;

int client_init(Client * client, long qid);
Client * client_create();
void client_delete(Client * client);


typedef struct ClientList
{
    int client_count;
    Client ** clients;
}   ClientList;

int cl_init(ClientList * cl);
int cl_add(ClientList * cl, long qid);
void cl_clean(ClientList * cl);
bool is_valid_id(ClientList * cl, long qid);
bool is_busy(ClientList * cl, long cid);


void cleanup(void);
void remove_queue(void);
void handle_sigint(int signo);
int handle_init(char * buf, size_t size, ClientList * cl);
int handle_list(char * buf, size_t size, ClientList * cl);
int handle_stop(char * buf, size_t size, ClientList * cl);
int handle_connect(char * buf, size_t size, ClientList * cl);



int main(int argc, char * argv[])
{
    if (atexit(cleanup) != 0) err("atexit failed", __FILE__, __func__, __LINE__);
    if (signal(SIGINT, handle_sigint) == SIG_ERR) syserr("singal", __FILE__, __func__, __LINE__);


    /* stworzenie kolejki i wypisanie jej klucza na stdout */
    SRVR_FTOK_KEY = FTOK_SERVER_ID;
    const char * ENV_HOME = getenv("HOME");
    if (!ENV_HOME) err("Failed to fetch $HOME variable", __FILE__, __func__, __LINE__);

    if ((SRVR_Q_KEY = ftok(ENV_HOME, SRVR_FTOK_KEY)) < 0) syserr("ftok failed", __FILE__, __func__, __LINE__);

    while (SRVR_FTOK_KEY < 254 && (SRVR_PUB_Q_ID = msgget(SRVR_Q_KEY, IPC_CREAT | IPC_EXCL | 0666)) < 0) 
    {
        int errnum = errno;
        if (errnum == EEXIST)
        {
            ++SRVR_FTOK_KEY;
            if ((SRVR_Q_KEY = ftok(ENV_HOME, SRVR_FTOK_KEY)) < 0) syserr("ftok failed", __FILE__, __func__, __LINE__);
        }
        else 
            syserr("msgget failed", __FILE__, __func__, __LINE__);
    }
    if (SRVR_Q_KEY == 254) err("msgget failed", __FILE__, __func__, __LINE__);

    fprintf(stdout, "SERVER PUBLIC QUEUE ID = %d\n", SRVR_PUB_Q_ID);


    ClientList client_list; cl_init(&client_list);

    Message msg;
    size_t msg_size;
    while (true)
    {
        if ((msg_size = msgrcv(SRVR_PUB_Q_ID, &msg, MAX_MSG_LEN, 0, 0)) < 0) syserr("msgrcv", __FILE__, __func__, __LINE__);

        switch(msg.type)
        {
            case MT_INIT:
            {
                handle_init(msg.buf, msg_size, &client_list);
                break;
            }
            case MT_LIST:
            {
                handle_list(msg.buf, msg_size, &client_list);
                break;
            }
            case MT_STOP:
            {
                handle_stop(msg.buf, msg_size, &client_list);
                break;
            }
            default:
            {
                printf("DEFAULT IN SWITCH\n");
            }

        }


        printf("Received message: %s\n", msg.buf);
    }

    // free(fdset);
    exit(EXIT_SUCCESS);
}


void cleanup(void)
{
    remove_queue();
}


void remove_queue(void)
{
    if (SRVR_PUB_Q_ID != -1 && msgctl(SRVR_PUB_Q_ID, IPC_RMID, NULL) < 0) 
        syserr_noexit("failed to remove server public queue", __FILE__, __func__, __LINE__);
}


void handle_sigint(int signo)
{
    if (signo != SIGINT) return;
    exit(EXIT_SUCCESS);
}


int client_init(Client * client, long qid)
{
    if (!client) return -1;
    client->busy = false;
    client->queue_id = qid;
    return 0;
}

Client * client_create()
{
    return (Client *) calloc(1, sizeof(Client));
}

void client_delete(Client * client)
{
    if (!client) return;
    free(client);
}

int cl_init(ClientList * cl)
{
    if (!cl) return -1;
    cl->client_count = 0;
    cl->clients = (Client **) calloc(MAX_CLIENTS, sizeof(Client *));
    if (!cl->clients) return -1;
    return 0;
}

void cl_clear(ClientList * cl) 
{
    if (!cl) return;
    for (int i = 0; i < MAX_CLIENTS; ++i) client_delete(cl->clients[i]);
    free(cl->clients);
}


int cl_add(ClientList * cl, long qid)
{
    if (!cl || qid <= 0 || cl->client_count >= MAX_CLIENTS) return -1;
    Client * new_client = client_create();
    if (!new_client) syserr("calloc failed to allocate memory for new client", __FILE__, __func__, __LINE__);

    for (int i = 0; i < MAX_CLIENTS; ++i)
    {
        if ((cl->clients[i]) == NULL)
        {
            client_init(new_client, qid);
            cl->clients[i] = new_client;
            cl->client_count++;
            return i;
        }
    }
    return -1;
}


int handle_init(char * buf, size_t size, ClientList * cl)
{
    if (!buf || size <= 0) return -1;
    long client_queue_id = strtol(buf, NULL, 10);
    if (client_queue_id <= 0) return -1;

    long client_id;
    if ((client_id = cl_add(cl, client_queue_id)) < 0) err("failed to add client", __FILE__, __func__, __LINE__);

    char buf2[MAX_MSG_LEN]; clearbuf(buf2, MAX_MSG_LEN);
    if (sprintf(buf2, "%ld", client_id) <= 0) err("failed to convert client id to string, sprintf", __FILE__, __func__, __LINE__);
    Message msg; 
    set_message(&msg, MT_INIT, buf2);

    if (msgsnd(client_queue_id, &msg, strlen(buf2), 0) < 0) syserr("msgsnd failed", __FILE__, __func__, __LINE__);
    
    return 0;
}


int handle_list(char * buf, size_t size, ClientList * cl)
{
    if (!buf || size <= 0 || !cl) return -1;

    long client_id = strtol(strtok(buf, " "), NULL, 10);
    if (!is_valid_id(cl, client_id)) 
    {
        err_noexit("invalid id received from client", __FILE__, __func__, __LINE__);
        return -1;
    }

    char buf2[MAX_MSG_LEN]; clearbuf(buf2, MAX_MSG_LEN);
    char buf16[16];
    char ch;

    for (int i = 0; i < MAX_CLIENTS; ++i)
    {
        if (cl->clients[i] != NULL)
        {
            clearbuf(buf16, 16);
            ch = (cl->clients[i]->busy) ? '-' : '+';
            sprintf(buf16, "%d%c\n", i, ch);
            strcat(buf2, buf16);
        }
    }

    Message msg; 
    set_message(&msg, MT_LIST, buf2);
    if (msgsnd(cl->clients[client_id]->queue_id, &msg, strlen(msg.buf), 0) < 0) syserr("msgsnd failed", __FILE__, __func__, __LINE__);

    return 0;
}


int handle_stop(char * buf, size_t size, ClientList * cl)
{
    if (!buf || size <= 0 || !cl) return -1;

    long client_id = strtol(strtok(buf, " "), NULL, 10);
    if (!is_valid_id(cl, client_id)) 
    {
        err_noexit("invalid id received from client", __FILE__, __func__, __LINE__);
        return -1;
    }

    client_delete(cl->clients[client_id]);
    cl->clients[client_id] = NULL;
    cl->client_count--;
    return 0;
}

int handle_connect(char * buf, size_t size, ClientList * cl)
{
    if (!buf || size <= 0 || !cl) return -1;

    char * token1 = strtok(buf, " ");
    char * token2 = strtok(NULL, " ");
    if (!token1 || !token2) 
    {
        err_noexit("expected two ids", __FILE__, __func__, __LINE__);
        return -1;
    }
    long client_id = strtol(token1, NULL, 10);
    long client2_id = strtol(token2, NULL, 10);
    if (is_busy(cl, client2_id) || is_busy(cl, client_id)) 
    {
        err_noexit("invalid id received from client or one of them is busy", __FILE__, __func__, __LINE__);
        return -1;
    }


    Message msg; 
    char buf16[16]; clearbuf(buf16, 16);

    if (sprintf(buf16, "%ld", cl->clients[client2_id]->queue_id) < 0) 
    {
        err_noexit("failed to convert qid to string", __FILE__, __func__, __LINE__);
        return -1;
    }
    set_message(&msg, MT_CONNECT, token1);
    if (msgsnd(cl->clients[client_id]->queue_id, &msg, strlen(msg.buf), 0) < 0) syserr("msgsnd failed", __FILE__, __func__, __LINE__);

    clearbuf(buf16, 16);
    if (sprintf(buf16, "%ld", cl->clients[client_id]->queue_id) < 0) 
    {
        err_noexit("failed to convert qid to string", __FILE__, __func__, __LINE__);
        return -1;
    }
    if (msgsnd(cl->clients[client2_id]->queue_id, &msg, strlen(msg.buf), 0) < 0) syserr("msgsnd failed", __FILE__, __func__, __LINE__);
    return 0;
}

bool is_valid_id(ClientList * cl, long cid)
{
    return (cid >= 0 && cl->clients[cid] != NULL);
}

bool is_busy(ClientList * cl, long cid)
{
    if (!cl || !is_valid_id(cl, cid)) return false;
    return cl->clients[cid]->busy;
}