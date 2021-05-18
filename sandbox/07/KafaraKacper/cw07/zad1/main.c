#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <wait.h>
#include "util.h"
#include "constants.h"

#define CLOCK_CONV 1000000

typedef
union semun 
{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
}   semun_u;



int OWEN_SEM_ID = -1;
int TABLE_SEM_ID = -1;
int OWEN_SHM_ID = -1;
int TABLE_SHM_ID = -1;
int * OWEN_SHM_PTR = NULL;
int * TABLE_SHM_PTR = NULL;
pid_t cpid = 1;

/**
 * Przyjmujemy dwie liczby N i M
 * 
 * N -- liczba kucharzy
 * M -- liczba dostawców
 */

void launch_cook();
void launch_delivery();
void handle_sigint(int signo);
void cleanup();


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

    /* pobranie zmiennej $HOME */
    char * ENV_HOME;
    if ((ENV_HOME = getenv("HOME")) == NULL)
        syserr("getenv", __FILE__, __func__, __LINE__);

    /* generujemy klucze */
    key_t sem_owen_key, sem_table_key;
    if ((sem_owen_key = ftok(ENV_HOME, SEM_OWEN_PROJ_ID)) < 0)
        syserr("ftok owen", __FILE__, __func__, __LINE__);
    if ((sem_table_key = ftok(ENV_HOME, SEM_TABLE_PROJ_ID)) < 0)
        syserr("ftok table", __FILE__, __func__, __LINE__);
        
    /* tworzymy semafory */
    /**
     * w pierwszym semaforze trzymamy flagę dostępu (1 jeżeli można modyfikować pamięć)
     * w drugim semaforze trzymamy liczbę wolnych miejsc w piecu
     */
    if ((OWEN_SEM_ID = semget(sem_owen_key, 2, IPC_CREAT | IPC_EXCL | OPEN_RW_ALL)) < 0)
        syserr("semget", __FILE__, __func__, __LINE__);

    /**
     * w pierwszym semaforze trzymamy flagę dostępu (1 jeżeli można modyfikować pamięć)
     * w drugim semaforze trzymamy liczbę wolnych miejsc na stole
     */
    if ((TABLE_SEM_ID = semget(sem_table_key, 2, IPC_CREAT | IPC_EXCL | OPEN_RW_ALL)) < 0)
        syserr("semget", __FILE__, __func__, __LINE__);

    
    /* inicjalizujemy semafory */
    semun_u arg;
    arg.array = (unsigned short *) malloc(2 * sizeof(unsigned short));
    arg.array[0] = 1;
    arg.array[1] = PZ_OWEN_SIZE;
    
    if ((semctl(OWEN_SEM_ID, 0, SETALL, arg)) < 0)
        syserr("semctl", __FILE__, __func__, __LINE__);

    arg.array[0] = 1;
    arg.array[1] = PZ_TABLE_SIZE;
    if ((semctl(TABLE_SEM_ID, 0, SETALL, arg)) < 0)
        syserr("semctl", __FILE__, __func__, __LINE__);
        
    free(arg.array);

    /* tworzymy obszar pamięci współdzielonej */
    key_t shm_owen_key, shm_table_key;
    if ((shm_owen_key = ftok(ENV_HOME, SHM_OWEN_PROJ_ID)) < 0)
        syserr("ftok shm_owen", __FILE__, __func__, __LINE__);

    if ((shm_table_key = ftok(ENV_HOME, SHM_TABLE_PROJ_ID)) < 0)
        syserr("ftok shm_table", __FILE__, __func__, __LINE__);

    if ((OWEN_SHM_ID = shmget(shm_owen_key, PZ_OWEN_SIZE * sizeof(int), IPC_CREAT | IPC_EXCL | OPEN_RW_ALL)) < 0)
        syserr("shmget", __FILE__, __func__, __LINE__);

    if ((OWEN_SHM_PTR = shmat(OWEN_SHM_ID, 0, 0)) < 0)
        syserr("shmat", __FILE__, __func__, __LINE__);

    if ((TABLE_SHM_ID = shmget(shm_table_key, (PZ_TABLE_SIZE + 1) * sizeof(int), IPC_CREAT | IPC_EXCL | OPEN_RW_ALL)) < 0)
        syserr("shmget", __FILE__, __func__, __LINE__);
        
    if ((TABLE_SHM_PTR = shmat(TABLE_SHM_ID, 0, 0)) < 0)
        syserr("shmat", __FILE__, __func__, __LINE__);

    for (int i = 0; i < PZ_OWEN_SIZE; ++i) OWEN_SHM_PTR[i] = -1;
    for (int i = 0; i < PZ_TABLE_SIZE; ++i) TABLE_SHM_PTR[i] = -1;
    TABLE_SHM_PTR[PZ_TABLE_SIZE] = N * PZ_NUM; // liczba pizz;

    /* tworzymy kucharzy */
    for (int i = 0; i < N; ++i)
        launch_cook();

    /* tworzymy dostawców */
    for (int i = 0; i < M; ++i)
        launch_delivery();

    printf("MAIN: Waiting for all children to finish...\n");
    pid_t cpid;
    while ((cpid = waitpid(-1, NULL, 0)) != -1)
        printf("MAIN: %d finished\n", cpid);
    printf("MAIN: all children have finished\n");

    exit(EXIT_SUCCESS);
}

void launch_cook()
{
    pid_t cpid;
    if ((cpid = fork()) == 0)
    {
        char * args[] = {"cook", NULL};
        execv("./cook", args);
        syserr("exec cook", __FILE__, __func__, __LINE__);
    }
    else if (cpid < 0)
        syserr("fork cook", __FILE__, __func__, __LINE__);
}

void launch_delivery()
{
    pid_t cpid;
    if ((cpid = fork()) == 0)
    {
        char * args[] = {"delivery", NULL};
        execv("./delivery", args);
        syserr("exec delivery", __FILE__, __func__, __LINE__);
    }
    else if (cpid < 0)
        syserr("fork delivery", __FILE__, __func__, __LINE__);

}

void cleanup()
{
    printf("PID:%d: closing semaphore\n", getpid());
    if (OWEN_SEM_ID != -1)
        if (semctl(OWEN_SEM_ID, 0, IPC_RMID) < 0)
            syserr("semctl", __FILE__, __func__, __LINE__);
    if (TABLE_SEM_ID != -1)
        if (semctl(TABLE_SEM_ID, 0, IPC_RMID) < 0)
            syserr("semctl", __FILE__, __func__, __LINE__);

    if (OWEN_SHM_PTR != NULL && OWEN_SHM_ID != -1)
    {
        printf("PID:%d: closing shared memory (owen)\n", getpid());
        if (shmdt(OWEN_SHM_PTR) < 0)
            syserr("schmdt", __FILE__, __func__, __LINE__);
        if (cpid > 0)
            if (shmctl(OWEN_SHM_ID, IPC_RMID, NULL) < 0)
                syserr("shmctl", __FILE__, __func__, __LINE__);
    }

    if (TABLE_SHM_PTR != NULL && TABLE_SHM_ID != -1)
    {
        printf("PID:%d: closing shared memory (table)\n", getpid());
        if (shmdt(TABLE_SHM_PTR) < 0)
            syserr("shmdt", __FILE__, __func__, __LINE__);
        if (cpid > 0)
            if (shmctl(TABLE_SHM_ID, IPC_RMID, NULL) < 0)
                syserr("shmctl", __FILE__, __func__, __LINE__);
    }
}

void handle_sigint(int signo)
{
    exit(EXIT_SUCCESS);
}