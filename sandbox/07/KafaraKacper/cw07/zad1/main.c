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

void cook();
void delivery();
void cleanup();
int find_empty_index(int arr[], int size);
int find_busy_index(int arr[], int size);

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
    if ((OWEN_SEM_ID = semget(sem_owen_key, 3, IPC_CREAT | IPC_EXCL | OPEN_RW_ALL)) < 0)
        syserr("semget", __FILE__, __func__, __LINE__);

    if ((TABLE_SEM_ID = semget(sem_table_key, 3, IPC_CREAT | IPC_EXCL | OPEN_RW_ALL)) < 0)
        syserr("semget", __FILE__, __func__, __LINE__);

    
    /* inicjalizujemy semafory */
    semun_u arg;
    arg.array = (unsigned short *) malloc(3 * sizeof(unsigned short));
    arg.array[0] = 1;
    arg.array[1] = 0;
    arg.array[2] = PZ_OWEN_SIZE; 
    
    if ((semctl(OWEN_SEM_ID, 0, SETALL, arg)) < 0)
        syserr("semctl", __FILE__, __func__, __LINE__);

    arg.array[0] = 1;
    arg.array[1] = 0;
    arg.array[2] = PZ_TABLE_SIZE;
    if ((semctl(TABLE_SEM_ID, 0, SETALL, arg)) < 0)
        syserr("semctl", __FILE__, __func__, __LINE__);
        
    // for (int i = 0; i < 3; ++i) arg.array[i] = -2;

    // semctl(TABLE_SEM_ID, 0, GETALL, arg);
    // for (int i = 0; i < 3; ++i) printf("%d ", arg.array[i]);
    // printf("\n");
    free(arg.array);

    /* tworzymy obszar pamięci współdzielonej */
    key_t shm_owen_key, shm_table_key;
    if ((shm_owen_key = ftok(ENV_HOME, SHM_OWEN_PROJ_ID)) < 0)
        syserr("ftok shm_owen", __FILE__, __func__, __LINE__);

    if ((shm_table_key = ftok(ENV_HOME, SHM_TABLE_PROJ_ID)) < 0)
        syserr("ftok shm_table", __FILE__, __func__, __LINE__);

    if ((OWEN_SHM_ID = shmget(IPC_PRIVATE, PZ_OWEN_SIZE * sizeof(int), OPEN_RW_ALL)) < 0)
        syserr("shmget", __FILE__, __func__, __LINE__);

    int * array;
    if ((array = shmat(OWEN_SHM_ID, 0, 0)) < 0)
        syserr("shmat", __FILE__, __func__, __LINE__);

    for (int i = 0; i < PZ_OWEN_SIZE; ++i) array[i] = -1;

    if ((TABLE_SHM_ID = shmget(IPC_PRIVATE, PZ_TABLE_SIZE * sizeof(int), OPEN_RW_ALL)) < 0)
        syserr("shmget", __FILE__, __func__, __LINE__);
        
    if ((array = shmat(TABLE_SHM_ID, 0, 0)) < 0)
        syserr("shmat", __FILE__, __func__, __LINE__);

    for (int i = 0; i < PZ_TABLE_SIZE; ++i) array[i] = -1;

    // czy ja powienienm pozamykać te obszary pamięci? 

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
    // struct sembuf set_next_index;
    /* zamknięcie semafora */
    put_pizza[0].sem_num = take_pizza[0].sem_num = 0;
    put_pizza[0].sem_flg = take_pizza[0].sem_flg = 0;
    put_pizza[0].sem_op  = take_pizza[0].sem_op  = -1;
    /* pomniejszenie liczby wolnych miejsc */
    put_pizza[1].sem_num = take_pizza[1].sem_num = 2;
    put_pizza[1].sem_flg = take_pizza[1].sem_flg = 0;
    put_pizza[1].sem_op  = -1;
    take_pizza[1].sem_op = 1;
    /* ustawienie kolejnego wolnego indeksu w pamięci współdzielonej */
    // set_next_index.sem_num     = 1;
    // set_next_index.sem_flg     = 0;
    // semb[2].sem_op     ;; to ustawiamy w pętli
    /* zwolnienie semafora */
    release.sem_num     = 0;
    release.sem_flg     = 0;
    release.sem_op      = 1;
    put_pizza_on_table[0].sem_num = 0;
    put_pizza_on_table[0].sem_flg = 0;
    put_pizza_on_table[0].sem_op  = -1;
    put_pizza_on_table[1].sem_num = 2;
    put_pizza_on_table[1].sem_flg = 0;
    put_pizza_on_table[1].sem_op  = -1;

    semun_u value;
    int empty_index;
    
    // czy semafor w dziecku jest otwarty? 
    if ((OWEN_SHM_PTR = shmat(OWEN_SHM_ID, 0, 0)) < 0)
        syserr("shmat", __FILE__, __func__, __LINE__);

    
    for (int i = 0; i < PZ_NUM; ++i) 
    {
        printf("%d:cook:iteracja %d\n", getpid(), i);
        semun_u arg;
        arg.array = (unsigned short *) malloc(3 * sizeof(unsigned short));
        for (int i = 0; i < 3; ++i) arg.array[i] = -2;
        semctl(TABLE_SEM_ID, 0, GETALL, arg);
        printf("%d: getall\n", getpid());
        for (int i = 0; i < 3; ++i) printf("%d ", arg.array[i]);
        printf("\n");
        free(arg.array);
        pizza_type = rand() % (PZT_MAX + 1);
        // trzeba jeszcze wypisać timestamp
        clock_gettime(CLOCK_REALTIME, &ts_curtime);
        printf("%d:(%ld:%ld):cook: Przygotowuje %d\n", cook_id, ts_curtime.tv_sec, ts_curtime.tv_nsec / CLOCK_CONV,pizza_type);
        ts_sleeptime.tv_nsec = rand() % (int)(1e8);
        if (nanosleep(&ts_sleeptime2, NULL) < 0)
            err_noexit("cook: nanosleep failed.", __FILE__, __func__, __LINE__);

        if (semctl(OWEN_SEM_ID, 2, GETVAL, value) < 0)
            syserr("semctl", __FILE__, __func__, __LINE__);
        



        printf("value returned by GET: %d\n", value.val);

        clock_gettime(CLOCK_REALTIME, &ts_curtime);
        printf("%d:(%ld:%ld):cook: Dodaje pizze %d, liczba pizz w piecu %d\n", cook_id, ts_curtime.tv_sec, ts_curtime.tv_nsec / CLOCK_CONV,pizza_type, PZ_OWEN_SIZE - value.val);
        /**
         * 1. Zamykamy pierwszy semafor (nikt inny nie będzie mógł wykonać operacji)
         * 2. Pomniejszenie liczby wolnych miejsc 
         */
        if (semop(OWEN_SEM_ID, put_pizza, 2) < 0)
            syserr("semop", __FILE__, __func__, __LINE__);

        /* aktualizacja pamięci współdzielonej */
        if (semctl(OWEN_SEM_ID, 1, GETVAL, value) < 0)
            syserr("semctl", __FILE__, __func__, __LINE__);

        empty_index = value.val;
        printf("%d, EMPTY INDEX %d, value: %d\n", getpid(), empty_index, OWEN_SHM_PTR[empty_index]);

        if (OWEN_SHM_PTR[empty_index] != -1)
            err("MECHANISM FAILED", __FILE__, __func__, __LINE__);
                
        OWEN_SHM_PTR[empty_index] = pizza_type;
        value.val = find_empty_index(OWEN_SHM_PTR, PZ_OWEN_SIZE);
        printf("%d, empty returned %d\n", getpid(), value.val);

        if (semctl(OWEN_SEM_ID, 1, SETVAL, value) < 0)
            syserr("semop", __FILE__, __func__, __LINE__);

        if (semop(OWEN_SEM_ID, &release, 1) < 0)
            syserr("semop", __FILE__, __func__, __LINE__);

        /* czekamy na upieczenie */
        ts_sleeptime.tv_nsec = rand() % (int)(1e8);
        if (nanosleep(&ts_sleeptime, NULL) < 0)
            err_noexit("cook: nanosleep failed.", __FILE__, __func__, __LINE__);
            
        // wyjęcie pizzy z pieca i umieszczenie jej na stole do wysyłki 
        // poinformowanie o tym jaką pizzę wyjmuje, obecna liczba pizz w piecu, 
        // obecna liczba pizz na stole 

        /* najpierw wyjmujemy pizzę z pieca */
        if (semop(OWEN_SEM_ID, take_pizza, 2) < 0)
            syserr("semop", __FILE__, __func__, __LINE__);

        /* pamiętamy pod jaki indeks wstawiliśmy */
        OWEN_SHM_PTR[empty_index] = -1;
        clock_gettime(CLOCK_REALTIME, &ts_curtime);
        printf("%d:(%ld:%ld):cook: Wyciagam pizze %d\n", cook_id, ts_curtime.tv_sec, ts_curtime.tv_nsec / CLOCK_CONV,pizza_type);

        /* potrzebujemy jeszcze uaktualnić najniższy pusty indeks*/ 
        value.val = find_empty_index(OWEN_SHM_PTR, PZ_OWEN_SIZE);
        if (value.val == -1) 
            err_noexit("There is no place in owen!", __FILE__, __func__, __LINE__);

        /* zamykamy piec */
        if (semop(OWEN_SEM_ID, &release, 1) < 0)
            syserr("semop", __FILE__, __func__, __LINE__);

        /* wstawiamy teraz pizze na stol do zabrania */
        if (semop(TABLE_SEM_ID, put_pizza_on_table, 2) < 0)
            syserr("semop", __FILE__, __func__, __LINE__);

        if (semctl(TABLE_SEM_ID, 2, GETVAL, value) < 0)
            syserr("semctl", __FILE__, __func__, __LINE__);

        TABLE_SHM_PTR[value.val] = pizza_type;
        value.val = find_empty_index(TABLE_SHM_PTR, PZ_TABLE_SIZE);
        value.val = (value.val >= 0) ? (value.val) : 0;

        if (semctl(TABLE_SEM_ID, 1, SETVAL, value) < 0)
            syserr("semctl", __FILE__, __func__, __LINE__);

        if (semop(TABLE_SEM_ID, &release, 1) < 0)
            syserr("semop", __FILE__, __func__, __LINE__);
    }
    exit(EXIT_SUCCESS);
}

void cleanup()
{
    if (cpid > 0)
    {
        printf("PID:%d: closing semaphore\n", getpid());
        if (OWEN_SEM_ID != -1)
            if (semctl(OWEN_SEM_ID, 0, IPC_RMID) < 0)
                syserr("semctl", __FILE__, __func__, __LINE__);
        if (TABLE_SEM_ID != -1)
            if (semctl(TABLE_SEM_ID, 0, IPC_RMID) < 0)
                syserr("semctl", __FILE__, __func__, __LINE__);
    }

    if (OWEN_SHM_PTR != NULL && OWEN_SHM_ID != -1)
    {
        printf("PID:%d: closing shared memory (owen)\n", getpid());
        if (shmctl(OWEN_SHM_ID, IPC_RMID, NULL) < 0)
            syserr("shmctl", __FILE__, __func__, __LINE__);
    }

    if (TABLE_SHM_PTR != NULL && TABLE_SHM_ID != -1)
    {
        printf("PID:%d: closing shared memory (table)\n", getpid());
        if (shmctl(TABLE_SHM_ID, IPC_RMID, NULL) < 0)
            syserr("shmctl", __FILE__, __func__, __LINE__);
    }
}

void delivery()
{
    printf("%d:delivery: Started\n", getpid()); 
    srand(time(NULL));
    struct timespec ts_delivery_time;
    struct timespec ts_curtime;
    ts_delivery_time.tv_sec = 4;
    semun_u value;

    struct sembuf take_pizza[2];
    take_pizza[0].sem_num   = take_pizza[0].sem_flg = 0;
    take_pizza[0].sem_op    = -1;
    take_pizza[1].sem_num   = 2;
    take_pizza[1].sem_flg   = 0;
    take_pizza[1].sem_op    = 1;

    struct sembuf release;
    release.sem_flg = release.sem_num = 0;
    release.sem_op = 1;
    int busy_index;
    int pizza_type;

    for (int i = 0; i < 10; ++i)
    {
        printf("%d:delivery: Iteracja %d\n", getpid(), i);
        if (semctl(TABLE_SEM_ID, 2, GETVAL, value) < 0)
            syserr("semctl", __FILE__, __func__, __LINE__);

        
        printf("%d:delivery: Iteracja %d, wartosc sem. 2: %d\n", getpid(), i, value.val);
        
        if (value.val == PZ_TABLE_SIZE)
        {
            printf("%d:delivery:iteracja %d: Koncze przez break\n", getpid(), i);
            break;
        }

        if (semop(TABLE_SEM_ID, take_pizza, 2) < 0)
            syserr("semop", __FILE__, __func__, __LINE__);
        

        busy_index = find_busy_index(TABLE_SHM_PTR, PZ_TABLE_SIZE);
        pizza_type = TABLE_SHM_PTR[busy_index];
        TABLE_SHM_PTR[busy_index] = -1;
        value.val = find_empty_index(TABLE_SHM_PTR, PZ_TABLE_SIZE);
        
        clock_gettime(CLOCK_REALTIME, &ts_curtime);
        printf("%d:(%ld:%ld):delivery: Pobranie pizzy %d, ze stolu\n", getpid(), ts_curtime.tv_sec, ts_curtime.tv_nsec / CLOCK_CONV, pizza_type);

        if (semctl(TABLE_SEM_ID, 1, SETVAL, value) < 0)
            syserr("semctl", __FILE__, __func__, __LINE__);

        if (semop(TABLE_SEM_ID, &release, 1) < 0)
            syserr("semop", __FILE__, __func__, __LINE__);
            
        ts_delivery_time.tv_sec = rand() % (int)(1e8);
        if (nanosleep(&ts_delivery_time, NULL) < 0)
            err_noexit("delivery: nanosleep failed.", __FILE__, __func__, __LINE__);
            
        clock_gettime(CLOCK_REALTIME, &ts_curtime);
        printf("%d:(%ld:%ld):delivery: Dostarczenie pizzy %d\n", getpid(), ts_curtime.tv_sec, ts_curtime.tv_nsec / CLOCK_CONV, pizza_type);
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

int find_empty_index(int arr[], int size)
{
    for (int i = 0; i < size; ++i)
        if (arr[i] == -1)
            return i;
    return -1;
}

int find_busy_index(int arr[], int size)
{
    for (int i = 0; i < size; ++i)
        if (arr[i] != -1)
            return i;
    return -1;
}