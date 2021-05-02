#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <poll.h>
#include <wait.h>
#include <time.h>
#include "util.h"
#include "constants.h"


int CLIENT_Q_DS     = -1;
int SERVER_Q_DS     = -1;
int CHAT_QUEUE_DS   = -1;
int CLIENT_ID       = -1;
char QNAME_BUF[MAX_QNAME_LEN];
char CHAT_QNAME[MAX_QNAME_LEN];


void cleanup(void);
void remove_queue(void);
void handle_sigint(int signo);
int handle_user_input(char * buf, size_t size, long serverqid, long myid);
int handle_queue_input(char * buf, size_t size, long serverqid, long myid);
int create_listener(int pipefd[2], long qid, pid_t * cpid);
void generate_qname(char * buf, size_t bufsize);



int main(int argc, char * argv[])
{
    clearbuf(CHAT_QNAME, MAX_QNAME_LEN);
    srand(time(NULL));

    if (atexit(cleanup) != 0) err("atexit failed", __FILE__, __func__, __LINE__);
    if (signal(SIGINT, handle_sigint) == SIG_ERR) syserr("signal", __FILE__, __func__, __LINE__);

    /* stworzenie kolejki */ 
    generate_qname(QNAME_BUF, MAX_QNAME_LEN - 1);
    int attempts = 0;
    while ((CLIENT_Q_DS = mq_open(QNAME_BUF, O_CREAT, O_EXCL | 0666, NULL)) < 0 && attempts < 50)
    {
        if (errno == EEXIST) 
        {
            err_noexit("queue with given name already exists; trying with new name...", __FILE__, __func__, __LINE__);
            generate_qname(QNAME_BUF, MAX_QNAME_LEN - 1);
            ++attempts;
        }
        else
            syserr("mq_open failed", __FILE__, __func__, __LINE__);
    }
    if (attempts >= 50) 
        err("failed to create client's queue", __FILE__, __func__, __LINE__);

    if ((SERVER_Q_DS = mq_open(SERVER_QUEUE_NAME, O_WRONLY)) < 0)
        syserr("mq_open", __FILE__, __func__, __LINE__);

    printf("server descriptor: %d, client descriptor: %d\n", SERVER_Q_DS, CLIENT_Q_DS);

    /* wysylamy do serwera polecenie z danymi do rejestracji */
    char buf[MAX_MSG_LEN];
    if (mq_send(SERVER_Q_DS, QNAME_BUF, strlen(QNAME_BUF), MT_UBOUND - MT_INIT) < 0)
        syserr("mq_send", __FILE__, __func__, __LINE__);
        
    clearbuf(buf, MAX_MSG_LEN);

    /* odbieramy swoje id od serwera */
    long msgsize;
    if ((msgsize = mq_receive(CLIENT_Q_DS, buf, MAX_MSG_LEN, NULL)) < 0)
        syserr("mq_receive", __FILE__, __func__, __LINE__);

    if ((CLIENT_ID = strtol(buf, NULL, 10)) < 0) err("invalid id returned from server", __FILE__, __func__, __LINE__);
    printf("id assigned by server: %s\n", buf);
    
    struct pollfd stdinfd[2];
    stdinfd[0].fd = STDIN_FILENO;
    stdinfd[0].events = POLLIN;
    stdinfd[1].fd = CLIENT_Q_DS;
    stdinfd[1].events = POLLIN;
    int event_count, len;
    unsigned int prio;
    ssize_t read_bytes;
    char msgtype_buf[5]; 

    while (true)
    {
        if ((event_count = poll(stdinfd, 2, -1)) < 0) syserr("poll failed", __FILE__, __func__, __LINE__);
            
        if (stdinfd[0].revents & POLLIN) 
        {
            clearbuf(buf, MAX_MSG_LEN);
            if ((read_bytes = read(STDIN_FILENO, buf, MAX_MSG_LEN)) <= 0) err("invalid input", __FILE__, __func__, __LINE__);
            len = strlen(buf);
            if (buf[len-1] == '\n') buf[len-1] = 0;
            handle_user_input(buf, strlen(buf), SERVER_Q_DS, CLIENT_ID);
        }

        if (stdinfd[1].revents & POLLIN)
        {
            clearbuf(buf, MAX_MSG_LEN);
            clearbuf(msgtype_buf, 5);
            if ((read_bytes = mq_receive(CLIENT_Q_DS, buf, MAX_MSG_LEN, &prio)) < 0)
                syserr("mq_receive", __FILE__, __func__, __LINE__);

            if (sprintf(msgtype_buf, " %d", prio) < 0) 
                err("failed to convert prio into string", __FILE__, __func__, __LINE__);

            strcat(buf, msgtype_buf);
            printf("buf in main loop: %s\n", buf);
            handle_queue_input(buf, read_bytes + strlen(msgtype_buf), SERVER_Q_DS, CLIENT_ID);
        }
    }
    exit(EXIT_SUCCESS);
}


void remove_queue(void)
{
    if (CLIENT_Q_DS != -1)
    {
        if (mq_close(CLIENT_Q_DS) < 0 || mq_unlink(QNAME_BUF) < 0)
            syserr("failed to close / remove client queue", __FILE__, __func__, __LINE__);
    }
    if (SERVER_Q_DS != -1)
    {
        if (mq_close(SERVER_Q_DS) < 0)
            syserr("failed to close server queue", __FILE__, __func__, __LINE__);
    }
    if (CHAT_QUEUE_DS != -1)
    {
        if (mq_close(CHAT_QUEUE_DS) < 0)
            syserr("failed to close converser queue", __FILE__, __func__, __LINE__);
    }
}


void handle_sigint(int signo)
{
    if (signo != SIGINT) return;
    exit(EXIT_SUCCESS);
}


void cleanup(void)
{
    if (CLIENT_Q_DS != -1)
    {
        char idbuf[10]; clearbuf(idbuf, 10);
        sprintf(idbuf, "%d", CLIENT_ID);
        if (mq_send(SERVER_Q_DS, idbuf, 10, MT_UBOUND - MT_STOP) < 0)
            syserr("mq_send", __FILE__, __func__, __LINE__);
        remove_queue();
    } 
}


int handle_user_input(char * buf, size_t size, long serverqid, long myid)
{
    if (!buf || size == 0 || serverqid < 0) return -1;
    char * token = strtok(buf, " ");
    if (!token) return -1;

    char idbuf[10]; clearbuf(idbuf, 10);
    sprintf(idbuf, "%ld", myid);

    if (strcmp(token, "LIST") == 0) 
    {
        if (mq_send(serverqid, idbuf, 10, MT_UBOUND - MT_LIST) < 0)
            syserr("mq_send", __FILE__, __func__, __LINE__);
    }
    else if (strcmp(token, "STOP") == 0)
    {
        // wiadomosc do serwera jest wysylana w funkcji zarejestrowanej w atexit
        exit(EXIT_SUCCESS);
    }
    else if (strcmp(token, "CONNECT") == 0)
    {
        strcat(idbuf, " "); 
        if ((token = strtok(NULL, " ")) == NULL) 
        {
            err_noexit("expected id after CONNECT; no action", __FILE__, __func__, __LINE__);
            return -1;
        }
        strcat(idbuf, token);
        if (mq_send(serverqid, idbuf, 10, MT_UBOUND - MT_CONNECT) < 0)
            syserr("mq_send", __FILE__, __func__, __LINE__);
    }
    else if (strcmp(token, "DISCONNECT") == 0 && CHAT_QUEUE_DS != -1)
    {
        printf("before sending message in disconnect\n");
        if (mq_send(serverqid, idbuf, 10, MT_UBOUND - MT_DISCONNECT) < 0)
            syserr("mq_send", __FILE__, __func__, __LINE__);
        printf("after sending message in disconnect\n");
        clearbuf(CHAT_QNAME, MAX_MSG_LEN);
        fprintf(stdout, "disconnected\n");
    }
    else if (CHAT_QUEUE_DS != -1) 
    {
        if (mq_send(CHAT_QUEUE_DS, buf, strlen(buf), MT_UBOUND - MT_CHAT) < 0)
            syserr("mq_send", __FILE__, __func__, __LINE__);
    }
    else 
    {
        fprintf(stdout, "unrecognized command / message\n");
    }
    return 0;

}

int handle_queue_input(char * buf, size_t size, long serverqid, long myid)
{
    printf("hci: %s\n", buf);
    char * message = strtok(buf, " ");
    char * msgtype_buf = strtok(NULL, " ");
    printf("%s, %s\n", message, msgtype_buf);
    if (!msgtype_buf || !message) return -1;
    long msgtype = strtol(msgtype_buf, NULL, 10);


    switch(MT_UBOUND - msgtype)
    {
        case MT_CONNECT:
        {
            printf("connecting\n");
            clearbuf(CHAT_QNAME, MAX_QNAME_LEN);
            strcpy(CHAT_QNAME, message);
            if ((CHAT_QUEUE_DS = mq_open(CHAT_QNAME, O_WRONLY)) < 0) 
                syserr("mq_open", __FILE__, __func__, __LINE__);
            fprintf(stdout, "connected\n");
            break;
        }
        case MT_STOP:
        {        
            exit(EXIT_SUCCESS);
            break;
        }
        case MT_CHAT:
        {
            if (CHAT_QUEUE_DS == -1) break;
            fprintf(stdout, "received(%d): %s\n", CHAT_QUEUE_DS, message);
            fflush(stdout);
            break;
        }
        case MT_LIST:
        {
            fprintf(stdout, "%s",message);
            break;
        }
        case MT_DISCONNECT:
        {
            CHAT_QUEUE_DS = -1;
            clearbuf(CHAT_QNAME, MAX_QNAME_LEN);
            fprintf(stdout, "disconnected\n");
            break;
        }
    }
    return 0;
}

void generate_qname(char * buf, size_t bufsize)
{
    static char base[] = "QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm123456789";
    buf[0] = '/';
    for (size_t i = 1; i < bufsize - 1; ++i)
        buf[i] = base[rand() % 61];
    buf[bufsize] = 0;
}