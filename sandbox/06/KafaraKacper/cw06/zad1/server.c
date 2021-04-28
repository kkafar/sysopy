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
// #include <sys/ipc.h>
// #include <linux/msg.h

int SRVR_PUB_Q_ID = -1;
int SRVR_FTOK_KEY = -1;
key_t SRVR_Q_KEY  = -1;

void cleanup(void);
void remove_queue(void);
void handle_sigint(int signo);




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

    fprintf(stdout, "SERVER PUBLIC QUEUE: (key, id) = (%d, %d)\n", SRVR_Q_KEY, SRVR_PUB_Q_ID);

    Message msg;
    // fd_set * fdset = (fd_set *) malloc(sizeof(fd_set));
    // FD_ZERO(fdset);
    // FD_SET(STDIN_FILENO, fdset);
    while (true)
    {
        // select(1, fdset, NULL, NULL, NULL);
        if (msgrcv(SRVR_PUB_Q_ID, &msg, MAX_MSG_LEN, 0, 0) < 0) syserr("msgrcv", __FILE__, __func__, __LINE__);
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