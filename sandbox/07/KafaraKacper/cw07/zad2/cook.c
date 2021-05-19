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
#include <time.h>
#include <unistd.h>

#define CLOCK_CONV 1000000

void handle_sigint(int signo);
void cleanup();
int find_busy_index(int arr[], int size);
int find_next_empty_cell(int arr[], int size, int start);

sem_t * sem_owen_ptr = NULL;
sem_t * sem_table_ptr = NULL;
int * shm_owen_ptr = NULL;
int * shm_table_ptr = NULL;
int fd_shm_owen = -1;
int fd_shm_table = -1;

int main(int argc, char * argv[])
{
    if (atexit(cleanup) < 0)
        syserr("atexit failed", __FILE__, __func__, __LINE__);

    if (signal(SIGINT, handle_sigint) == SIG_ERR)
        syserr("signal", __FILE__, __func__, __LINE__);
    
    if ((sem_owen_ptr = sem_open(SEM_OWEN_NAME, O_RDWR)) == SEM_FAILED)
        syserr("sem_open owen", __FILE__, __func__, __LINE__);

    if ((sem_table_ptr = sem_open(SEM_TABLE_NAME, O_RDWR)) == SEM_FAILED)
        syserr("sem_open table", __FILE__, __func__, __LINE__);

    if ((fd_shm_owen = shm_open(SHM_OWEN_NAME, O_RDWR, 0666)) < 0) 
        syserr("shm_open owen", __FILE__, __func__, __LINE__);

    if (ftruncate(fd_shm_owen, (PZ_OWEN_SIZE + 1) * sizeof(int)) < 0)
        syserr("ftruncate owen", __FILE__, __func__, __LINE__);

    /* zmapowanie do pamięci procesu */
    if ((shm_owen_ptr = (int *) mmap(
        NULL, 
        (PZ_OWEN_SIZE + 1) * sizeof(int), 
        PROT_WRITE | PROT_READ | PROT_EXEC, 
        MAP_SHARED,
        fd_shm_owen,
        0)) == MAP_FAILED) 
        syserr("mmap owen", __FILE__, __func__, __LINE__);


    if ((fd_shm_table = shm_open(SHM_TABLE_NAME, O_RDWR, 0666)) < 0)
        syserr("shm_open table", __FILE__, __func__, __LINE__);

    // /* rezerwacja obszaru */
    if (ftruncate(fd_shm_table, (PZ_TABLE_SIZE + 2) * sizeof(int)) < 0)
        syserr("ftruncate table", __FILE__, __func__, __LINE__);

    if ((shm_table_ptr = (int *) mmap(
        NULL, 
        (PZ_TABLE_SIZE + 2) * sizeof(int), 
        PROT_WRITE | PROT_READ | PROT_EXEC, 
        MAP_SHARED,
        fd_shm_table,
        0)) == MAP_FAILED) 
        syserr("mmap table", __FILE__, __func__, __LINE__); 


    printf("%d:cook: Started\n", getpid());
    pid_t cook_id = getpid();
    srand(cook_id);
    int pizza_type, *empty_slots, empty_index = 0, table_empty_index = 0;
    struct timespec ts_sleeptime, ts_sleeptime2, ts_curtime, ts_retry_penalty;
    ts_sleeptime2.tv_sec = 2;
    ts_sleeptime.tv_sec = 4;
    ts_retry_penalty.tv_sec = 0;
    ts_retry_penalty.tv_nsec = 500000000; // 0.5 s

    for (int i = 0; i < PZ_NUM; ++i)
    {
        pizza_type = rand() % (PZT_MAX + 1);
        clock_gettime(CLOCK_REALTIME, &ts_curtime);
        printf(
            "%d:(%ld:%ld):cook: Przygotowuje %d\n", 
            cook_id, 
            ts_curtime.tv_sec, 
            ts_curtime.tv_nsec / CLOCK_CONV, 
            pizza_type);   

        ts_sleeptime2.tv_nsec = rand() % (long)(1e8);
        if (nanosleep(&ts_sleeptime2, NULL) < 0)
            syserr("cook: nanosleep failed.", __FILE__, __func__, __LINE__);

        // próbujemy dostać się do pieca
        while (1)
        {
            printf("cook inner loop\n");
            if (sem_wait(sem_owen_ptr) < 0) 
                syserr("sem_wait", __FILE__, __func__, __LINE__); 
            
            printf("opened owen\n");
            empty_slots = &shm_owen_ptr[PZ_OWEN_SIZE];
            // dostaliśmy się, sprawdzamy czy jest miejsce
            if (*empty_slots <= 0) 
            {
                // skoro nie ma miejsca to zwalniamy piec
                if (sem_post(sem_owen_ptr) < 0)
                    syserr("sem_post", __FILE__, __func__, __LINE__); 

                if (nanosleep(&ts_retry_penalty, NULL) < 0)
                    err_noexit("cook: nanosleep failed.", __FILE__, __func__, __LINE__);
                continue;
            }
        }
        /**
         * W piecu jest miejsce
         * 
         * 1. zmniejszamy liczbę dosępnych miejsc 
         * 2. wstawiamy pizzę do pieca
         * 3. zwalniamy piec 
         */
        (*empty_slots)--;

        clock_gettime(CLOCK_REALTIME, &ts_curtime);
        printf("%d:(%ld:%ld):cook: Dodaje pizze %d, liczba pizz w piecu %d\n", cook_id, ts_curtime.tv_sec, ts_curtime.tv_nsec / CLOCK_CONV,pizza_type, PZ_OWEN_SIZE - *empty_slots);

        empty_index = find_next_empty_cell(shm_owen_ptr, PZ_OWEN_SIZE, empty_index);
        if (empty_index < 0 || shm_owen_ptr[empty_index] != -1)
            printf("EMPTY INDEX < 0 || value != -1\n");

        shm_owen_ptr[empty_index] = pizza_type;

        if (sem_post(sem_owen_ptr) < 0)
            syserr("sem_post", __FILE__, __func__, __LINE__); 


        // czekamy na upieczenie
        ts_sleeptime.tv_nsec = rand() % (int)(1e8);
        if (nanosleep(&ts_sleeptime, NULL) < 0)
            err_noexit("cook: nanosleep failed.", __FILE__, __func__, __LINE__);

        // próbujemy wyciągnąć pizzę z pieca
        if (sem_wait(sem_owen_ptr) < 0)
            syserr("sem_wait", __FILE__, __func__, __LINE__); 

        shm_owen_ptr[empty_index] = -1;
        // zwiększamy liczbę wolnych pizz w piecu
        (*empty_slots)++;
        clock_gettime(CLOCK_REALTIME, &ts_curtime);
        printf(
            "%d:(%ld:%ld):cook: Wyciagam pizze %d, pozostalo w piecu %d\n", 
            cook_id, 
            ts_curtime.tv_sec, ts_curtime.tv_nsec / CLOCK_CONV,
            pizza_type,
            PZ_OWEN_SIZE - *empty_slots);

        // zamykamy piec
        if (sem_post(sem_owen_ptr) < 0)
            syserr("sem_post", __FILE__, __func__, __LINE__); 

        // teraz próbujemy wstawić pizzę na stół
        while (1)
        {
            if (sem_wait(sem_table_ptr) < 0)
                syserr("sem_wait", __FILE__, __func__, __LINE__); 

            // sprawdzamy czy jest wolne miejsce na stole 
            if (shm_table_ptr[PZ_TABLE_SIZE] <= 0)
            {
                if (sem_post(sem_table_ptr) < 0)
                    syserr("sem_post", __FILE__, __func__, __LINE__); 

                if (nanosleep(&ts_retry_penalty, NULL) < 0)
                    syserr("nanosleep", __FILE__, __func__, __LINE__); 
                continue;
            }
        }

        // jest miejsce na stole
        table_empty_index = find_next_empty_cell(shm_table_ptr, PZ_TABLE_SIZE, table_empty_index);
        /* wstawiamy pizzę i zmniejszamy liczbę wolnych miejsc w piecu */
        shm_table_ptr[table_empty_index] = pizza_type;
        shm_table_ptr[PZ_TABLE_SIZE]--; 
        clock_gettime(CLOCK_REALTIME, &ts_curtime);
        printf(
            "%d:(%ld:%ld):cook: Umieszczam pizze %d na stole, obecnie na stole %d\n", 
            cook_id, 
            ts_curtime.tv_sec, ts_curtime.tv_nsec / CLOCK_CONV,
            pizza_type,
            PZ_TABLE_SIZE - shm_table_ptr[PZ_TABLE_SIZE]);

        // zamykamy stół
        if (sem_post(sem_table_ptr) < 0)
            syserr("sem_post", __FILE__, __func__, __LINE__); 
    }

    exit(EXIT_SUCCESS);
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
        
        // if (sem_unlink(SEM_OWEN_NAME) < 0)
        //     syserr("sem_unlink owen", __FILE__, __func__, __LINE__);
    }
    if (sem_table_ptr != NULL)
    {
        if (sem_close(sem_table_ptr) < 0)
            syserr("sem_close table", __FILE__, __func__, __LINE__);
        
        // if (sem_unlink(SEM_TABLE_NAME) < 0)
        //     syserr("sem_unlink table", __FILE__, __func__, __LINE__);
    }
    if (shm_owen_ptr != NULL) 
    {
        if (munmap(shm_owen_ptr, (PZ_OWEN_SIZE + 1) * sizeof(int)) < 0)
            syserr("munmap owen", __FILE__, __func__, __LINE__);

        // if (shm_unlink(SHM_OWEN_NAME) < 0)
        //     syserr("shm_unlink owen", __FILE__, __func__, __LINE__);
    }
    if (shm_table_ptr != NULL) 
    {
        if (munmap(shm_table_ptr, (PZ_TABLE_SIZE + 2) * sizeof(int)) < 0)
            syserr("munmap table", __FILE__, __func__, __LINE__);

        // if (shm_unlink(SHM_TABLE_NAME) < 0)
        //     syserr("shm_unlink table", __FILE__, __func__, __LINE__);
    }
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
