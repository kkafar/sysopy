#include <stdlib.h>
#include <stdio.h>
#include "util.h"
#include "constants.h"
#include <semaphore.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include <sys/mman.h>


void launch_cook();
void launch_delivery();
void handle_sigint(int signo);
void cleanup();
void generate_string(char * buf, size_t bufsize);


sem_t * sem_owen_ptr = NULL;
sem_t * sem_table_ptr = NULL;



int main(int argc, char * argv[])
{
    printf("MAIN: PID: %d\n", getpid());
    if (argc != 3)
        err("Invalid number of arguments", __FILE__, __func__, __LINE__);
        
    if (atexit(cleanup) < 0)
        syserr("atexit failed", __FILE__, __func__, __LINE__);

    if (signal(SIGINT, handle_sigint) == SIG_ERR)
        syserr("signal", __FILE__, __func__, __LINE__);
        
    long N, M;
    if ((N = strtol(argv[1], NULL, 10)) <= 0)  
        err("Invalid number of cooks.", __FILE__, __func__, __LINE__);
    if ((M = strtol(argv[2], NULL, 10)) <= 0)
        err("Invalid number of delivery guys.", __FILE__, __func__, __LINE__);

    /* tworzenie semaforów */
    if ((sem_owen_ptr = sem_open(SEM_OWEN_NAME, IPC_CREAT | IPC_EXCL, 0666, 1)) == SEM_FAILED)
        syserr("sem_open owen", __FILE__, __func__, __LINE__);

    if ((sem_table_ptr = sem_open(SEM_TABLE_NAME, IPC_CREAT | IPC_EXCL, 0666, 1)))
        syserr("sem_open table", __FILE__, __func__, __LINE__);




    exit(EXIT_SUCCESS);
}


void generate_string(char * buf, size_t bufsize)
{
    static char base[] = "QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm123456789";
    buf[0] = '/';
    for (size_t i = 1; i < bufsize - 1; ++i)
        buf[i] = base[rand() % 61];
    buf[bufsize] = 0;
}


void handle_sigint(int signo)
{
    exit(EXIT_SUCCESS);
}

void cleanup()
{
    if (sem_owen_ptr != NULL)
    {
        if (sem_close(sem_owen_ptr) < 0)
            syserr("sem_close owen", __FILE__, __func__, __LINE__);
        
        if (sem_unlink(SEM_OWEN_NAME) < 0)
            syserr("sem_unlink owen", __FILE__, __func__, __LINE__);
    }
    if (sem_table_ptr != NULL)
    {
        if (sem_close(sem_table_ptr) < 0)
            syserr("sem_close table", __FILE__, __func__, __LINE__);
        
        if (sem_unlink(SEM_TABLE_NAME) < 0)
            syserr("sem_unlink table", __FILE__, __func__, __LINE__);
    }
}