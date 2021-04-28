#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/select.h>
#include "util.h"
#include "constants.h"


int CLIENT_Q_ID = -1;


void cleanup(void);
void remove_queue(void);
void handle_sigint(int signo);





int main(int argc, char * argv[])
{
    if (atexit(cleanup) != 0) err("atexit failed", __FILE__, __func__, __LINE__);
    if (signal(SIGINT, handle_sigint) == SIG_ERR) syserr("signal", __FILE__, __func__, __LINE__);

    if (argc != 2) err("bad arg count; expected only server's queue id", __FILE__, __func__, __LINE__);

    long server_q_id = strtol(argv[1], NULL, 10);
    if (server_q_id < 0) err("invalid server queue id; must be >= 0", __FILE__, __func__, __LINE__);

    /* w jaki sposob mam wybrac klucz dla klienta?? */ 
    const char * ENV_HOME = getenv("HOME");
    if (!ENV_HOME) err("failed to fetch $HOME variable", __FILE__, __func__, __LINE__);

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

    



    exit(EXIT_SUCCESS);
}


void remove_queue(void)
{
    if (CLIENT_Q_ID != -1 && msgctl(CLIENT_Q_ID, IPC_RMID, NULL) < 0)
        syserr("failed to remove client queue", __FILE__, __func__, __LINE__);
}


void handle_sigint(int signo)
{
    if (signo != SIGINT) return;
    exit(EXIT_SUCCESS);
}


void cleanup(void)
{
    remove_queue();
}