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


    /* pobranie zmiennej $HOME */
    char * ENV_HOME;
    if ((ENV_HOME = getenv("HOME")) == NULL)
        syserr("getenv", __FILE__, __func__, __LINE__);

    /* generujemy klucze - mają być takie same jak w mainie*/
    key_t sem_owen_key, sem_table_key;
    if ((sem_owen_key = ftok(ENV_HOME, SEM_OWEN_PROJ_ID)) < 0)
        syserr("ftok owen", __FILE__, __func__, __LINE__);
    if ((sem_table_key = ftok(ENV_HOME, SEM_TABLE_PROJ_ID)) < 0)
        syserr("ftok table", __FILE__, __func__, __LINE__);
        
    /* podbieramy id semaforow */
    /**
     * w pierwszym semaforze trzymamy flagę dostępu (1 jeżeli można modyfikować pamięć)
     * w drugim semaforze trzymamy liczbę wolnych miejsc w piecu
     */
    if ((OWEN_SEM_ID = semget(sem_owen_key, 2, 0)) < 0)
        syserr("semget", __FILE__, __func__, __LINE__);

    /**
     * w pierwszym semaforze trzymamy flagę dostępu (1 jeżeli można modyfikować pamięć)
     * w drugim semaforze trzymamy liczbę wolnych miejsc na stole
     */
    if ((TABLE_SEM_ID = semget(sem_table_key, 2, 0)) < 0)
        syserr("semget", __FILE__, __func__, __LINE__);

    /* podłączamy się do obszaru pamięci współdzielonej */
    key_t shm_owen_key, shm_table_key;
    if ((shm_owen_key = ftok(ENV_HOME, SHM_OWEN_PROJ_ID)) < 0)
        syserr("ftok shm_owen", __FILE__, __func__, __LINE__);

    if ((shm_table_key = ftok(ENV_HOME, SHM_TABLE_PROJ_ID)) < 0)
        syserr("ftok shm_table", __FILE__, __func__, __LINE__);

    if ((OWEN_SHM_ID = shmget(shm_owen_key, 0, OPEN_RW_ALL)) < 0)
        syserr("shmget", __FILE__, __func__, __LINE__);

    if ((OWEN_SHM_PTR = shmat(OWEN_SHM_ID, 0, 0)) < 0)
        syserr("shmat", __FILE__, __func__, __LINE__);

    for (int i = 0; i < PZ_OWEN_SIZE; ++i) OWEN_SHM_PTR[i] = -1;

    if ((TABLE_SHM_ID = shmget(shm_table_key, 0, OPEN_RW_ALL)) < 0)
        syserr("shmget", __FILE__, __func__, __LINE__);
        
    if ((TABLE_SHM_PTR = shmat(TABLE_SHM_ID, 0, 0)) < 0)
        syserr("shmat", __FILE__, __func__, __LINE__);


    printf("%d:cook: Started\n", getpid());
    pid_t cook_id = getpid();
    srand(cook_id);
    int pizza_type; 
    struct timespec ts_sleeptime, ts_sleeptime2, ts_curtime;
    ts_sleeptime2.tv_sec = 2;
    ts_sleeptime.tv_sec = 4;

    struct sembuf put_pizza[2];
    struct sembuf take_pizza[2];
    struct sembuf put_pizza_on_table[2];
    struct sembuf release;
    /* zamknięcie semafora */
    put_pizza[0].sem_num = take_pizza[0].sem_num = 0;
    put_pizza[0].sem_flg = take_pizza[0].sem_flg = 0;
    put_pizza[0].sem_op  = take_pizza[0].sem_op  = -1;
    /* modyfikacja liczby wolnych miejsc */
    put_pizza[1].sem_num = take_pizza[1].sem_num = 1;
    put_pizza[1].sem_flg = take_pizza[1].sem_flg = 0;
    put_pizza[1].sem_op  = -1; // zajmujemy miejsce
    take_pizza[1].sem_op = 1; // zwalniamy miejsce
    /* zwolnienie semafora */
    release.sem_num     = 0;
    release.sem_flg     = 0;
    release.sem_op      = 1;
    put_pizza_on_table[0].sem_num = 0;
    put_pizza_on_table[0].sem_flg = 0;
    put_pizza_on_table[0].sem_op  = -1;
    put_pizza_on_table[1].sem_num = 1;
    put_pizza_on_table[1].sem_flg = 0;
    put_pizza_on_table[1].sem_op  = -1;

    semun_u value;
    int empty_index = 0;
    int table_empty_index = 0;
    
    // to powinien być ten sam wskaźnik co w rodzicu?
    if ((OWEN_SHM_PTR = shmat(OWEN_SHM_ID, 0, 0)) < 0)
        syserr("shmat", __FILE__, __func__, __LINE__);

    
    for (int i = 0; i < PZ_NUM; ++i) 
    {
        // printf("%d:cook:iteracja %d\n", cook_id, i);
        // semun_u arg;
        // arg.array = (unsigned short *) malloc(2 * sizeof(unsigned short));
        // for (int i = 0; i < 3; ++i) arg.array[i] = -1;
        // semctl(TABLE_SEM_ID, 0, GETALL, arg);
        // printf("%d: getall ", cook_id);
        // for (int i = 0; i < 2; ++i) printf("%d ", arg.array[i]);
        // printf("\n");
        // free(arg.array);

        pizza_type = rand() % (PZT_MAX + 1);
        // trzeba jeszcze wypisać timestamp
        clock_gettime(CLOCK_REALTIME, &ts_curtime);
        printf(
            "%d:(%ld:%ld):cook: Przygotowuje %d\n", 
            cook_id, 
            ts_curtime.tv_sec, 
            ts_curtime.tv_nsec / CLOCK_CONV, 
            pizza_type);
        
        ts_sleeptime.tv_nsec = rand() % (int)(1e8);
        if (nanosleep(&ts_sleeptime2, NULL) < 0)
            err_noexit("cook: nanosleep failed.", __FILE__, __func__, __LINE__);

        /**
         * 1. Zamykamy pierwszy semafor (nikt inny nie będzie mógł wykonać operacji)
         * 2. Pomniejszenie liczby wolnych miejsc 
         */
        if (semop(OWEN_SEM_ID, put_pizza, 2) < 0)
            syserr("semop", __FILE__, __func__, __LINE__);

        if ((value.val = semctl(OWEN_SEM_ID, 1, GETVAL)) < 0)
            syserr("semctl", __FILE__, __func__, __LINE__);
        
        clock_gettime(CLOCK_REALTIME, &ts_curtime);
        printf("%d:(%ld:%ld):cook: Dodaje pizze %d, liczba pizz w piecu %d\n", cook_id, ts_curtime.tv_sec, ts_curtime.tv_nsec / CLOCK_CONV,pizza_type, PZ_OWEN_SIZE - value.val);
        /* aktualizacja pamięci współdzielonej */
        empty_index = find_next_empty_cell(OWEN_SHM_PTR, PZ_OWEN_SIZE, empty_index);
        // printf("%d:cook:empty index found %d:value %d\n", getpid(), empty_index, OWEN_SHM_PTR[empty_index]);

        if (OWEN_SHM_PTR[empty_index] != -1)
            err("MECHANISM FAILED", __FILE__, __func__, __LINE__);
                
        OWEN_SHM_PTR[empty_index] = pizza_type;

        if (semop(OWEN_SEM_ID, &release, 1) < 0)
            syserr("semop", __FILE__, __func__, __LINE__);

        /* czekamy na upieczenie */
        ts_sleeptime.tv_nsec = rand() % (int)(1e8);
        if (nanosleep(&ts_sleeptime, NULL) < 0)
            err_noexit("cook: nanosleep failed.", __FILE__, __func__, __LINE__);
            
        /* najpierw wyjmujemy pizzę z pieca */
        if (semop(OWEN_SEM_ID, take_pizza, 2) < 0)
            syserr("semop", __FILE__, __func__, __LINE__);

        /* pamiętamy pod jaki indeks wstawiliśmy */
        OWEN_SHM_PTR[empty_index] = -1;

        /* pobieramy liczbę pizz w piecu */
        if ((value.val = semctl(OWEN_SEM_ID, 1, GETVAL)) < 0)
            syserr("semctl", __FILE__, __func__, __LINE__);

        clock_gettime(CLOCK_REALTIME, &ts_curtime);
        printf(
            "%d:(%ld:%ld):cook: Wyciagam pizze %d, pozostalo w piecu %d\n", 
            cook_id, 
            ts_curtime.tv_sec, ts_curtime.tv_nsec / CLOCK_CONV,
            pizza_type,
            PZ_OWEN_SIZE - value.val);

        /* zamykamy piec */
        if (semop(OWEN_SEM_ID, &release, 1) < 0)
            syserr("semop", __FILE__, __func__, __LINE__);

        /* wstawiamy teraz pizze na stol do zabrania */
        if (semop(TABLE_SEM_ID, put_pizza_on_table, 2) < 0)
            syserr("semop", __FILE__, __func__, __LINE__);


        table_empty_index = find_next_empty_cell(TABLE_SHM_PTR, PZ_TABLE_SIZE, table_empty_index);
        TABLE_SHM_PTR[table_empty_index] = pizza_type;
        // TABLE_SHM_PTR[PZ_TABLE_SIZE]--;

        if ((value.val = semctl(TABLE_SEM_ID, 1, GETVAL)) < 0)
            syserr("semctl", __FILE__, __func__, __LINE__);

        printf(
            "%d:(%ld:%ld):cook: Umieszczam pizze %d na stole, obecnie na stole %d\n", 
            cook_id, 
            ts_curtime.tv_sec, ts_curtime.tv_nsec / CLOCK_CONV,
            pizza_type,
            PZ_TABLE_SIZE - value.val);

        if (semop(TABLE_SEM_ID, &release, 1) < 0)
            syserr("semop", __FILE__, __func__, __LINE__);
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
    if (OWEN_SHM_PTR != NULL && OWEN_SHM_ID != -1)
    {
        printf("PID:%d: closing shared memory (owen)\n", getpid());
        if (shmdt(OWEN_SHM_PTR) < 0)
            syserr("schmdt", __FILE__, __func__, __LINE__);
    }

    if (TABLE_SHM_PTR != NULL && TABLE_SHM_ID != -1)
    {
        printf("PID:%d: closing shared memory (table)\n", getpid());
        if (shmdt(TABLE_SHM_PTR) < 0)
            syserr("shmdt", __FILE__, __func__, __LINE__);
    }
}