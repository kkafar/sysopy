#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <sys/msg.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdbool.h>
#include <sys/select.h>
#include <poll.h>
#include <wait.h>
#include "util.h"
#include "constants.h"


int CLIENT_Q_ID = -1;
pid_t CPID      = -1;
int SRVR_Q_ID   = -1;
int CHAT_QUEUE_ID = -1;
int CLIENT_ID      = -1;
// pid_t CPID2     = -1;
// bool CHATTING   = false;
// long CHAT_CLIENT_Q_ID = -1;
// int CHATPIPE[2];



void cleanup(void);
void remove_queue(void);
void handle_sigint(int signo);
int handle_user_input(char * buf, size_t size, long serverqid, long myid);
int handle_queue_input(char * buf, size_t size, long serverqid, long myid);
int create_listener(int pipefd[2], long qid, pid_t * cpid);




int main(int argc, char * argv[])
{
    if (atexit(cleanup) != 0) err("atexit failed", __FILE__, __func__, __LINE__);
    if (signal(SIGINT, handle_sigint) == SIG_ERR) syserr("signal", __FILE__, __func__, __LINE__);

    const char * ENV_HOME = getenv("HOME");
    if (!ENV_HOME) err("failed to fetch $HOME variable", __FILE__, __func__, __LINE__);

    /* mozna sie dostac do id kolejki serwera znajac dane na podstawie ktorych id bylo wygenerowane */
    // server_q_id = ftok(ENV_HOME, FTOK_SERVER_ID);
    long server_q_id;
    key_t server_q_key;
    if ((server_q_key = ftok(ENV_HOME, FTOK_SERVER_ID)) < 0) syserr("ftok failed", __FILE__, __func__, __LINE__);
    if ((server_q_id = msgget(server_q_key, 0)) < 0) syserr("failed to fetch server queue id", __FILE__, __func__, __LINE__);
    SRVR_Q_ID = server_q_id;

    key_t ftok_proj_id = FTOK_ID1;
    key_t ftok_key;
    if ((ftok_key = ftok(ENV_HOME, ftok_proj_id)) < 0) syserr("ftok failed", __FILE__, __func__, __LINE__);

    while (ftok_proj_id < 254 && (CLIENT_Q_ID = msgget(ftok_key, IPC_CREAT | IPC_EXCL | 0666)) < 0)
    {
        int errnum = errno;
        if (errnum == EEXIST)
        {
            ++ftok_proj_id;
            if ((ftok_key = ftok(ENV_HOME, ftok_proj_id)) < 0) syserr("ftok failed", __FILE__, __func__, __LINE__);
        }
        else 
            syserr("msgget failed", __FILE__, __func__, __LINE__);
    }
    if (ftok_proj_id == 255) err("was not able to create a message queue", __FILE__, __func__, __LINE__);

    /* wysylamy do serwera polecenie z danymi do rejestracji */
    char buf[MAX_MSG_LEN];
    clearbuf(buf, MAX_MSG_LEN);
    sprintf(buf, "%d", CLIENT_Q_ID);
    Message msg;
    set_message(&msg, MT_INIT, buf);
    if (msgsnd(server_q_id, &msg, strlen(msg.buf), 0) < 0) syserr("msgsnd failed", __FILE__, __func__, __LINE__);
    clearbuf(msg.buf, MAX_MSG_LEN);

    /* odbieramy swoje id od serwera */
    long msgsize;
    if ((msgsize = msgrcv(CLIENT_Q_ID, &msg, MAX_MSG_LEN, 0, 0)) < 0) syserr("msgrcv failed", __FILE__, __func__, __LINE__);

    long myid;
    if ((myid = strtol(msg.buf, NULL, 10)) < 0) err("invalid id returned from server", __FILE__, __func__, __LINE__);
    printf("id assigned by server: %s\n", msg.buf);
    CLIENT_ID = myid;
    

    // fd_set readfd; 
    // /* klient nigdy nie bedzie mial kolejki o kluczu 0 (STDIN_FILENO), poniewaz jest startowany
    // po serwerze ==> jezeli zaczynamy naliczac od poczatku to klucz 0 dostanie serwer */
    // FD_SET(STDIN_FILENO, &readfd);
    // FD_SET(CLIENT_Q_ID, &readfd);


    /* forkujemy sie, dziecko odpowiada za odbior komunikatow od serwera */
    int pipefd[2];
    if (pipe(pipefd) < 0) syserr("pipe", __FILE__, __func__, __LINE__);
    create_listener(pipefd, CLIENT_Q_ID, &CPID);


    struct pollfd stdinfd[2];
    stdinfd[0].fd = STDIN_FILENO;
    stdinfd[0].events = POLLIN;
    stdinfd[1].fd = pipefd[0];
    stdinfd[1].events = POLLIN;
    int event_count, len;
    ssize_t read_bytes;

    while (true)
    {
        if ((event_count = poll(stdinfd, 2, -1)) < 0) syserr("poll failed", __FILE__, __func__, __LINE__);
            
        if (stdinfd[0].revents & POLLIN) 
        {
            clearbuf(buf, MAX_MSG_LEN);
            if ((read_bytes = read(STDIN_FILENO, buf, MAX_MSG_LEN)) <= 0) err("invalid input", __FILE__, __func__, __LINE__);
            len = strlen(buf);
            if (buf[len-1] == '\n') buf[len-1] = 0;
            handle_user_input(buf, strlen(buf), server_q_id, myid);
        }

        if (stdinfd[1].revents & POLLIN)
        {
            clearbuf(buf, MAX_MSG_LEN);
            if ((read_bytes = read(pipefd[0], buf, MAX_MSG_LEN)) < 0) syserr("read", __FILE__, __func__, __LINE__);
            handle_queue_input(buf, read_bytes, server_q_id, myid);
        }
    }

    if (close(pipefd[0]) < 0) syserr("close", __FILE__, __func__, __LINE__);
    exit(EXIT_SUCCESS);
}


void remove_queue(void)
{
    if (CLIENT_Q_ID != -1 && CPID != 0 && msgctl(CLIENT_Q_ID, IPC_RMID, NULL) < 0)
        syserr("failed to remove client queue", __FILE__, __func__, __LINE__);
}


void handle_sigint(int signo)
{
    if (signo != SIGINT) return;
    exit(EXIT_SUCCESS);
}


void cleanup(void)
{
    if (CPID > 0) 
    {
        if (kill(CPID, SIGINT) < 0) syserr_noexit("failed to kill child", __FILE__, __func__, __LINE__);
        while (waitpid(-1, NULL, 0) != -1);
    }
        
    if (CPID != 0)
    {
        Message msg;
        char idbuf[10]; clearbuf(idbuf, 10);
        sprintf(idbuf, "%d", CLIENT_ID);
        set_message(&msg, MT_STOP, idbuf);
        if (msgsnd(SRVR_Q_ID, &msg, 10, 0) < 0) syserr("msgsnd failed", __FILE__, __func__, __LINE__);
        remove_queue();
    } 
}


int handle_user_input(char * buf, size_t size, long serverqid, long myid)
{
    if (!buf || size == 0 || serverqid < 0) return -1;
    char * token = strtok(buf, " ");
    if (!token) return -1;

    Message msg;
    char idbuf[10]; clearbuf(idbuf, 10);
    sprintf(idbuf, "%ld", myid);

    if (strcmp(token, "LIST") == 0) 
    {
        set_message(&msg, MT_LIST, idbuf);
        if (msgsnd(serverqid, &msg, 10, 0) < 0) syserr("msgsnd failed", __FILE__, __func__, __LINE__);
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
        set_message(&msg, MT_CONNECT, idbuf);
        if (msgsnd(serverqid, &msg, 10, 0) < 0) syserr("msgsnd failed", __FILE__, __func__, __LINE__);
    }
    else if (strcmp(token, "DISCONNECT") == 0 && CHAT_QUEUE_ID != -1)
    {
        set_message(&msg, MT_DISCONNECT, idbuf);
        if (msgsnd(serverqid, &msg, 10, 0) < 0) syserr("msgsnd failed", __FILE__, __func__, __LINE__);
        CHAT_QUEUE_ID = -1;
        fprintf(stdout, "disconnected\n");
    }
    else if (CHAT_QUEUE_ID != -1) 
    {
        set_message(&msg, MT_CHAT, buf);
        if (msgsnd(CHAT_QUEUE_ID, &msg, strlen(buf), 0) < 0) syserr("msgsnd failed", __FILE__, __func__, __LINE__);
    }
    else 
    {
        fprintf(stdout, "unrecognized command / message\n");
    }
    return 0;

}


int create_listener(int pipefd[2], long qid, pid_t * cpid) 
{
    ssize_t msgsize;
    Message msg; 
    char msg_type_buf[10];
    clearbuf(msg.buf, MAX_MSG_LEN);
    if ((*cpid = fork()) < 0) syserr("fork", __FILE__, __func__, __LINE__);
    else if (*cpid == 0)
    {
        if (close(pipefd[0]) < 0) syserr("close", __FILE__, __func__, __LINE__);

        ssize_t written_bytes;
        while (true)
        {
            if ((msgsize = msgrcv(qid, &msg, MAX_MSG_LEN, 0, 0)) < 0) syserr("msgrcv in child", __FILE__, __func__, __LINE__);
            strcat(msg.buf, " ");
            clearbuf(msg_type_buf, 10);
            sprintf(msg_type_buf, "%ld", msg.type);
            strcat(msg.buf, msg_type_buf);
            if ((written_bytes = write(pipefd[1], msg.buf, msgsize + 2)) <= 0) err("write failed in child", __FILE__, __func__, __LINE__);
            clearbuf(msg.buf, MAX_MSG_LEN);
        }
        
        if (close(pipefd[1]) < 0) syserr("close", __FILE__, __func__, __LINE__);
        exit(EXIT_SUCCESS);
    }
    if (close(pipefd[1]) < 0) syserr("close", __FILE__, __func__, __LINE__);
    return 0;

}

int handle_queue_input(char * buf, size_t size, long serverqid, long myid)
{
    char * message = strtok(buf, " ");
    char * msgtype_buf = strtok(NULL, " ");
    if (!msgtype_buf || !message) return -1;
    long msgtype = strtol(msgtype_buf, NULL, 10);
    
    switch(msgtype)
    {
        case MT_CONNECT:
        {
            CHAT_QUEUE_ID = strtol(message, NULL, 10);
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
            if (CHAT_QUEUE_ID == -1) break;
            fprintf(stdout, "received(%d): %s\n", CHAT_QUEUE_ID, message);
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
            CHAT_QUEUE_ID = -1;
            fprintf(stdout, "disconnected\n");
            break;
        }
    }
    return 0;
}