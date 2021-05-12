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

typedef
union semun 
{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
}   semun_u;



int SEM_ID = -1;
int SHM_ID = -1;
int * SHM_PTR = NULL;
pid_t cpid = 1;

/**
 * Przyjmujemy dwie liczby N i M
 * 
 * N -- liczba kucharzy
 * M -- liczba dostawców
 */

void cook();
void delivery();
void cleanup();


int main(int argc, char * argv[])
{
    if (argc != 3)
        err("Invalid number of arguments", __FILE__, __func__, __LINE__);
        
    if (atexit(cleanup) < 0)
        syserr("atexit failed", __FILE__, __func__, __LINE__);
        
    long N, M;
    if ((N = strtol(argv[1], NULL, 10)) <= 0)  
        err("Invalid number of cooks.", __FILE__, __func__, __LINE__);
    if ((M = strtol(argv[2], NULL, 10)) <= 0)
        err("Invalid number of delivery guys.", __FILE__, __func__, __LINE__);

        
    /* pozyskujemy zmienną środowiskową HOME */
    char * env_home;
    if ((env_home = getenv("HOME")) == NULL)
        syserr("getenv failed", __FILE__, __func__, __LINE__);

    /* uzyskujemy klucz dla semafora */
    key_t sem_key;
    if ((sem_key = ftok(env_home, N + M % 255 + 1)))

    /* tworzymy semafor */
    if ((SEM_ID = semget(sem_key, 2, IPC_CREAT | IPC_EXCL | 0666)) < 0)
        syserr("semget", __FILE__, __func__, __LINE__);
        
    semun_u arg;
    arg.array = (unsigned short *) malloc(2 * sizeof(unsigned short));
    arg.array[0] = arg.array[1] = 1; 
    if ((semctl(SEM_ID, 0, SETALL, arg)) < 0)
        syserr("semctl", __FILE__, __func__, __LINE__);
    free(arg.array);

    /* tworzymy obszar pamięci współdzielonej */
    if ((SHM_ID = shmget(IPC_PRIVATE, PZ_SHM_SIZE * sizeof(int), 0666)) < 0)
        syserr("shmget", __FILE__, __func__, __LINE__);

    /* podłączać się będziemy w dzieciach */

    for (int i = 0; i < N; ++i)
    {
        if ((cpid = fork()) == 0)
            cook();
        else if (cpid < 0)
            syserr("fork", __FILE__, __func__, __LINE__);
    }

    for (int i = 0; i < M; ++i)
    {
        if ((cpid = fork()) == 0)
            delivery();
        else if (cpid < 0)
            syserr("fork", __FILE__, __func__, __LINE__);
    }

    printf("MAIN: Waiting for all children to finish...\n");
    pid_t cpid;
    while ((cpid = waitpid(-1, NULL, 0)) != -1)
        printf("MAIN: %d finished\n", cpid);
    printf("MAIN: all children have finished\n");

    exit(EXIT_SUCCESS);
}

void cook()
{
    pid_t cook_id = getpid();
    srand(cook_id);
    int pizza_type; 
    struct timespec ts_sleeptime, ts_sleeptime2, ts_curtime;
    ts_sleeptime2.tv_sec = 1;
    ts_sleeptime.tv_sec = 2;
    // clock_gettime(CLOCK_REALTIME, &ts);

    // czy semafor w dziecku jest otwarty? 
    if ((SHM_PTR = shmat(SHM_ID, 0, 0)) < 0)
        syserr("shmat", __FILE__, __func__, __LINE__);

    
    for (int i = 0; i < PZ_NUM; ++i) 
    {
        pizza_type = rand() % (PZT_MAX + 1);
        // trzeba jeszcze wypisać timestamp
        clock_gettime(CLOCK_REALTIME, &ts_curtime);
        printf("%d:(%ld:%ld):cook: Przygotowuje %d\n", cook_id, ts_curtime.tv_sec, ts_curtime.tv_nsec / 10000000,pizza_type);
        ts_sleeptime.tv_nsec = rand() % (int)(1e8);
        if (nanosleep(&ts_sleeptime2, NULL) < 0)
            err_noexit("cook: nanosleep failed.", __FILE__, __func__, __LINE__);

        // umieszczenie pizzy w piecu
        // wypisanie komunikatu o tym fakcie oraz o aktualnej liczbie
        // pizz w piecu 
        printf("%d:(%ld:%ld):cook: Dodaje pizze %d\n", cook_id, ts_curtime.tv_sec, ts_curtime.tv_nsec / 10000000,pizza_type);

        // 
        ts_sleeptime.tv_nsec = rand() % (int)(1e8);
        if (nanosleep(&ts_sleeptime, NULL) < 0)
            err_noexit("cook: nanosleep failed.", __FILE__, __func__, __LINE__);
            
        // wyjęcie pizzy z pieca i umieszczenie jej na stole do wysyłki 
        // poinformowanie o tym jaką pizzę wyjmuje, obecna liczba pizz w piecu, 
        // obecna liczba pizz na stole 
    }
    exit(EXIT_SUCCESS);
}

void cleanup()
{
    if (cpid > 0 && SEM_ID != -1)
    {
        printf("PID:%d: closing semaphore\n", getpid());
        if (semctl(SEM_ID, 0, IPC_RMID) < 0)
            syserr("semctl", __FILE__, __func__, __LINE__);
    }
    if (SHM_PTR != NULL && SHM_ID != -1)
    {
        printf("PID:%d: closing shared memory\n", getpid());
        if (shmctl(SHM_ID, IPC_RMID, NULL) < 0)
            syserr("shmctl", __FILE__, __func__, __LINE__);
    }
}

void delivery()
{
    exit(EXIT_SUCCESS);
}