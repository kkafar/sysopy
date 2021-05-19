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
#include <errno.h>
#include <wait.h>
#include "util.h"
#include "constants.h"

#define CLOCK_CONV 1000000

int OWEN_SEM_ID = -1;
int TABLE_SEM_ID = -1;
int OWEN_SHM_ID = -1;
int TABLE_SHM_ID = -1;
int * OWEN_SHM_PTR = NULL;
int * TABLE_SHM_PTR = NULL;

typedef
union semun 
{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
}   semun_u;


void handle_sigint(int signo);
void cleanup();
int find_busy_index(int arr[], int size);
int find_next_empty_cell(int arr[], int size, int start);



int main(int argc, char * argv[])
{
    if (atexit(cleanup) < 0)
        syserr("atexit failed", __FILE__, __func__, __LINE__);

    if (signal(SIGINT, handle_sigint) == SIG_ERR)
        syserr("signal", __FILE__, __func__, __LINE__);

    printf("%d:delivery: Started\n", getpid()); 
    sleep(10);
    srand(time(NULL));

    /* pobranie zmiennej $HOME */
    char * ENV_HOME;
    if ((ENV_HOME = getenv("HOME")) == NULL)
        syserr("getenv", __FILE__, __func__, __LINE__);

    /* generujemy klucze - mają być takie same jak w mainie*/
    key_t sem_table_key;
    if ((sem_table_key = ftok(ENV_HOME, SEM_TABLE_PROJ_ID)) < 0)
        syserr("ftok table", __FILE__, __func__, __LINE__);
        
    /* podbieramy id semaforow */
    /**
     * w pierwszym semaforze trzymamy flagę dostępu (1 jeżeli można modyfikować pamięć)
     * w drugim semaforze trzymamy liczbę wolnych miejsc na stole
     */
    if ((TABLE_SEM_ID = semget(sem_table_key, 2, 0)) < 0)
        syserr("semget", __FILE__, __func__, __LINE__);

    /* podłączamy się do obszaru pamięci współdzielonej */
    key_t shm_table_key;
    if ((shm_table_key = ftok(ENV_HOME, SHM_TABLE_PROJ_ID)) < 0)
        syserr("ftok shm_table", __FILE__, __func__, __LINE__);

    if ((TABLE_SHM_ID = shmget(shm_table_key, 0, OPEN_RW_ALL)) < 0)
        syserr("shmget", __FILE__, __func__, __LINE__);
        
    if ((TABLE_SHM_PTR = shmat(TABLE_SHM_ID, 0, 0)) < 0)
        syserr("shmat", __FILE__, __func__, __LINE__);

    struct timespec ts_delivery_time;
    struct timespec ts_curtime;
    ts_delivery_time.tv_sec = 4;
    semun_u value;

    struct sembuf take_pizza[2];
    take_pizza[0].sem_num   = take_pizza[0].sem_flg = 0;
    take_pizza[0].sem_op    = -1;
    take_pizza[1].sem_num   = 1;
    take_pizza[1].sem_flg   = 0;
    take_pizza[1].sem_op    = 1;

    struct sembuf release;
    release.sem_flg = release.sem_num = 0;
    release.sem_op = 1;
    int busy_index;
    int pizza_type;

    while (1)
    {
        printf("%d:delivery:\n", getpid());
        if (semop(TABLE_SEM_ID, &take_pizza[0], 1) < 0)
            syserr("semop", __FILE__, __func__, __LINE__);

        if ((value.val = semctl(TABLE_SEM_ID, 1, GETVAL)) < 0)
            syserr("semctl", __FILE__, __func__, __LINE__);

        if (value.val == PZ_TABLE_SIZE) 
        {
            // printf(
            //     "%d:(%ld:%ld):delivery: Stol jest pusty\n", 
            //     getpid(), 
            //     ts_curtime.tv_sec, ts_curtime.tv_nsec / CLOCK_CONV);

            int pizzas_left = TABLE_SHM_PTR[PZ_TABLE_SIZE];

            if (semop(TABLE_SEM_ID, &release, 1) < 0)
                syserr("semop", __FILE__, __func__, __LINE__);
            
            if (pizzas_left == 0)
                exit(EXIT_SUCCESS);

            sleep(2);

            continue;
        }

        if (semop(TABLE_SEM_ID, &take_pizza[1], 1) < 0)
            syserr("semop", __FILE__, __func__, __LINE__);

        busy_index = find_busy_index(TABLE_SHM_PTR, PZ_TABLE_SIZE);
        pizza_type = TABLE_SHM_PTR[busy_index];
        TABLE_SHM_PTR[busy_index] = -1;
        TABLE_SHM_PTR[PZ_TABLE_SIZE]--;

        
        clock_gettime(CLOCK_REALTIME, &ts_curtime);
        printf(
            "%d:(%ld:%ld):delivery: Pobranie pizzy %d, ze stolu, na stole zostaje %d, ogolnie pozostalo %d\n", 
            getpid(), 
            ts_curtime.tv_sec, ts_curtime.tv_nsec / CLOCK_CONV, 
            pizza_type,
            PZ_TABLE_SIZE - value.val,
            TABLE_SHM_PTR[PZ_TABLE_SIZE]);

        if (semop(TABLE_SEM_ID, &release, 1) < 0)
            syserr("semop", __FILE__, __func__, __LINE__);
        
            
        ts_delivery_time.tv_nsec = rand() % (long)(1e8);
        if (nanosleep(&ts_delivery_time, NULL) < 0)
            err_noexit("delivery: nanosleep failed.", __FILE__, __func__, __LINE__);
            
        clock_gettime(CLOCK_REALTIME, &ts_curtime);
        printf("%d:(%ld:%ld):delivery: Dostarczenie pizzy %d\n", getpid(), ts_curtime.tv_sec, ts_curtime.tv_nsec / CLOCK_CONV, pizza_type);

        // powrót
        ts_delivery_time.tv_nsec = rand() % (long)(1e8);
        if (nanosleep(&ts_delivery_time, NULL) < 0)
            err_noexit("delivery: nanosleep failed.", __FILE__, __func__, __LINE__);
    }
    exit(EXIT_SUCCESS);
}

int find_next_empty_cell(int arr[], int size, int start)
{
    for (int i = 0; i < size; ++i)
        if (arr[(i + start) % size] == -1)
            return (i + start) % size;
    return -1;
}

int find_busy_index(int arr[], int size)
{
    for (int i = 0; i < size; ++i)
        if (arr[i] != -1)
            return i;
    return -1;
}

void handle_sigint(int signo)
{
    exit(EXIT_SUCCESS);
}

void cleanup()
{
    if (TABLE_SHM_PTR != NULL && TABLE_SHM_ID != -1)
    {
        printf("PID:%d: closing shared memory (table)\n", getpid());
        if (shmdt(TABLE_SHM_PTR) < 0)
            syserr("shmdt", __FILE__, __func__, __LINE__);
    }
}