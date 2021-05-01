#include <stdlib.h>
#include <stdio.h>
#include <mqueue.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <fcntl.h>
#include <stdbool.h>
#include <poll.h>
#include "util.h"
#include "constants.h"

mqd_t SERVER_Q_DES = -1;

typedef struct Client 
{
    long queue_id; 
    bool busy;
    int converser;
}   Client;

int client_init(Client * client, long qid);
Client * client_create();
void client_delete(Client * client);


typedef struct ClientList
{
    int client_count;
    Client ** clients;
}   ClientList;

ClientList * CLIENT_LIST = NULL;

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
int handle_disconnect(char * buf, size_t size, ClientList * cl);


int main(int argc, char * argv[])
{
    /* generowanie nazw kolejek */
    srand(time(NULL));

    if (atexit(cleanup) != 0) 
        syserr("atexit", __FILE__, __func__, __LINE__);
    if (signal(SIGINT, handle_sigint) == SIG_ERR) 
        syserr("singal", __FILE__, __func__, __LINE__);

    /* stworzenie kolejki */ 
    if ((SERVER_Q_DES = mq_open(SERVER_QUEUE_NAME, O_CREAT | O_EXCL | O_RDONLY)) < 0)
        syserr("mq_open failed", __FILE__, __func__, __LINE__);
    
    struct mq_attr qattr;
    if (mq_getattr(SERVER_Q_DES, &qattr) < 0)
        syserr("mq_getattr", __FILE__, __func__, __LINE__);
        
    qattr.mq_msgsize = MAX_MSG_LEN;

    if (mq_setattr(SERVER_Q_DES, &qattr, NULL) < 0)
        syserr("mq_setattr", __FILE__, __func__, __LINE__);

    ssize_t num_bytes;
    unsigned int prio;
    char buf[MAX_MSG_LEN]; clearbuf(buf, MAX_MSG_LEN);

    ClientList cl; cl_init(&cl);
    CLIENT_LIST = &cl;

    while (true)
    {
        if ((num_bytes = mq_receive(SERVER_Q_DES, buf, MAX_MSG_LEN, &prio)) < 0)
            syserr("mq_receive", __FILE__, __func__, __LINE__);

        switch (prio)
        {
            case MT_UBOUND - MT_INIT:
            {
                handle_init(buf, num_bytes, CLIENT_LIST);
                break;
            }
            case MT_UBOUND - MT_STOP:
            {
                handle_stop(buf, num_bytes, CLIENT_LIST);
                break;
            }
            case MT_UBOUND - MT_LIST:
            {
                handle_list(buf, num_bytes, CLIENT_LIST);
                break;
            }
            case MT_UBOUND - MT_CONNECT:
            {
                handle_connect(buf, num_bytes, CLIENT_LIST);
                break;
            }
            case MT_UBOUND - MT_DISCONNECT:
            {
                handle_disconnect(buf, num_bytes, CLIENT_LIST);
                break;
            }
            default:
            {
                err_noexit("unknown message type", __FILE__, __func__, __LINE__);
            }
        }
    }
    exit(EXIT_SUCCESS);
}


void remove_queue(void)
{
    if (SERVER_Q_DES != -1) 
    {
        if (mq_close(SERVER_Q_DES) < 0)
            syserr("mq_close failed", __FILE__, __func__, __LINE__);
        if (mq_unlink(SERVER_QUEUE_NAME) < 0)
            syserr("mq_unlink failed", __FILE__, __func__, __LINE__);
    }
}


void handle_sigint(int signo)
{
    if (signo != SIGINT) return;
    exit(EXIT_SUCCESS);
}

void cleanup(void)
{
    if (SERVER_Q_DES == -1) return;

    char buf[20] = "STOP";
    for (int i = 0; i < MAX_CLIENTS; ++i)
    {
        if (CLIENT_LIST->clients[i] != NULL)
        {
            if (mq_send(CLIENT_LIST->clients[i]->queue_id, buf, 6, MT_UBOUND - MT_STOP) < 0) 
                syserr("mq_send", __FILE__, __func__, __LINE__);

            // if (mq_close(CLIENT_LIST->clients[i]->queue_id) < 0)
            //     syserr("mq_close", __FILE__, __func__, __LINE__);
        }

    }

    ssize_t msg_size;
    while (CLIENT_LIST->client_count > 0)
    {
        if (mq_receive(SERVER_Q_DES, buf, 19, MT_UBOUND - MT_STOP) < 0)
                syserr("mq_receive", __FILE__, __func__, __LINE__);

        handle_stop(buf, msg_size, CLIENT_LIST);    
    }
    remove_queue();
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

bool is_valid_id(ClientList * cl, long cid)
{
    return (cid >= 0 && cl->clients[cid] != NULL);
}

bool is_busy(ClientList * cl, long cid)
{
    if (!cl || !is_valid_id(cl, cid)) return false;
    return cl->clients[cid]->busy;
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

    if (mq_send(client_queue_id, buf2, strlen(buf2), MT_UBOUND - MT_INIT) < 0) 
        syserr("mq_send", __FILE__, __func__, __LINE__);
    
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
    if (mq_send(cl->clients[client_id]->queue_id, buf2, strlen(buf2), MT_UBOUND - MT_LIST) < 0)
        syserr("mq_send", __FILE__, __func__, __LINE__);

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
    printf("connecting: client1: %ld, client2: %ld\n", client_id, client2_id);
    if (is_busy(cl, client2_id) || is_busy(cl, client_id) || client2_id == client_id) 
    {
        err_noexit("invalid id received from client or one of them is busy", __FILE__, __func__, __LINE__);
        return -1;
    }

    char buf16[16]; clearbuf(buf16, 16);
    if (sprintf(buf16, "%ld", cl->clients[client2_id]->queue_id) < 0) 
    {
        err_noexit("failed to convert qid to string", __FILE__, __func__, __LINE__);
        return -1;
    }

    if (mq_send(cl->clients[client_id]->queue_id, buf16, strlen(buf16), MT_UBOUND - MT_CONNECT) < 0)
        syserr("mq_send", __FILE__, __func__, __LINE__);

    clearbuf(buf16, 16);
    if (sprintf(buf16, "%ld", cl->clients[client_id]->queue_id) < 0) 
    {
        err_noexit("failed to convert qid to string", __FILE__, __func__, __LINE__);
        return -1;
    }

    if (mq_send(cl->clients[client2_id]->queue_id, buf16, strlen(buf16), MT_UBOUND - MT_CONNECT) < 0)
        syserr("mq_send", __FILE__, __func__, __LINE__);
    
    cl->clients[client_id]->busy = true;
    cl->clients[client_id]->converser = client2_id;
    cl->clients[client2_id]->busy = true;
    cl->clients[client2_id]->converser = client_id;
    return 0;
}


int handle_disconnect(char * buf, size_t size, ClientList * cl)
{
    if (!buf || size <= 0 || !cl) return -1;

    long client_id = strtol(strtok(buf, " "), NULL, 10);
    if (!is_valid_id(cl, client_id)) 
    {
        err_noexit("invalid id received from client", __FILE__, __func__, __LINE__);
        return -1;
    }

    int converser = cl->clients[client_id]->converser;
    if (mq_send(cl->clients[converser]->queue_id, "DISCONNECT", 12, MT_UBOUND - MT_DISCONNECT) < 0)
        syserr("mq_send", __FILE__, __func__, __LINE__);
        
    cl->clients[client_id]->busy = false;
    cl->clients[client_id]->converser = -1;
    cl->clients[converser]->busy = false;
    cl->clients[converser]->converser = -1;
    return 0;

}
