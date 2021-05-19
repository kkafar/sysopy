#include <stdlib.h>
#include <stdio.h>
#include "util.h"
#include "constants.h"
#include <semaphore.h>
#include <signal.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>


#define CLOCK_CONV 1000000

void handle_sigint(int signo);
void cleanup();
int find_busy_index(int arr[], int size);
int find_next_empty_cell(int arr[], int size, int start);
void logger(char msg[]);

// sem_t * sem_owen_ptr = NULL;
sem_t * sem_table_ptr = NULL;
// int * shm_owen_ptr = NULL;
int * shm_table_ptr = NULL;
// int fd_shm_owen = -1;
int fd_shm_table = -1;



int main(int argc, char * argv[])
{
    if (atexit(cleanup) < 0)
        syserr("atexit failed", __FILE__, __func__, __LINE__);

    if (signal(SIGINT, handle_sigint) == SIG_ERR)
        syserr("signal", __FILE__, __func__, __LINE__);
    
    if ((sem_table_ptr = sem_open(SEM_TABLE_NAME, O_RDWR)) == SEM_FAILED)
        syserr("sem_open table", __FILE__, __func__, __LINE__);

    /* zmapowanie do pamięci procesu */
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

    struct timespec ts_delivery_time;
    struct timespec ts_curtime, ts_retry_penalty;
    ts_retry_penalty.tv_sec = 0;
    ts_retry_penalty.tv_nsec = (long)(5e+8);
    ts_delivery_time.tv_sec = 4;   
    int busy_index, pizza_type, empty_slots, pizzas_left;

    sleep(8);

    while (1)
    {
        // probójemy dostać się do stolu
        while (1) 
        {
            // logger("czekam na otwarcie stolu");
            if (sem_wait(sem_table_ptr) < 0)
                syserr("sem_wait", __FILE__, __func__, __LINE__); 

            // logger("zajmuje stol");
            empty_slots = shm_table_ptr[PZ_TABLE_SIZE];
            pizzas_left = shm_table_ptr[PZ_TABLE_SIZE + 1];

            if (empty_slots == PZ_TABLE_SIZE)
            {

                if (sem_post(sem_table_ptr) < 0)
                    syserr("sem_post", __FILE__, __func__, __LINE__); 

                if (pizzas_left <= 0)
                    exit(EXIT_SUCCESS);
                
                if (nanosleep(&ts_retry_penalty, NULL) < 0)
                    err_noexit("cook: nanosleep failed.", __FILE__, __func__, __LINE__);

                // logger("na stole nie ma pizzy");
                continue;
            }
            // logger("jakas pizza jest na stole");
            break;
        }

        // dostaliśmy się do stołu oraz stół nie jest pusty
        busy_index = find_busy_index(shm_table_ptr, PZ_TABLE_SIZE);
        pizza_type = shm_table_ptr[busy_index];
        shm_table_ptr[busy_index] = -1;
        shm_table_ptr[PZ_TABLE_SIZE]++;
        shm_table_ptr[PZ_TABLE_SIZE + 1]--;

        clock_gettime(CLOCK_REALTIME, &ts_curtime);
        printf(
            "%d:(%ld:%ld):delivery: Pobranie pizzy %d, ze stolu, na stole zostaje %d, ogolnie pozostalo %d\n", 
            getpid(), 
            ts_curtime.tv_sec, ts_curtime.tv_nsec / CLOCK_CONV, 
            pizza_type,
            PZ_TABLE_SIZE - shm_table_ptr[PZ_TABLE_SIZE],
            shm_table_ptr[PZ_TABLE_SIZE + 1]);
        
        // zwalniamy stół
        if (sem_post(sem_table_ptr) < 0)
            syserr("sem_post", __FILE__, __func__, __LINE__); 

        ts_delivery_time.tv_nsec = rand() % (int)(1e8);
        if (nanosleep(&ts_delivery_time, NULL) < 0)
            err_noexit("delivery: nanosleep failed.", __FILE__, __func__, __LINE__);
            
        clock_gettime(CLOCK_REALTIME, &ts_curtime);
        printf("%d:(%ld:%ld):delivery: Dostarczenie pizzy %d\n", getpid(), ts_curtime.tv_sec, ts_curtime.tv_nsec / CLOCK_CONV, pizza_type);

        // powrót
        ts_delivery_time.tv_nsec = rand() % (int)(1e8);
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
    if (sem_table_ptr != NULL)
    {
        if (sem_close(sem_table_ptr) < 0)
            syserr("sem_close table", __FILE__, __func__, __LINE__);
        
        // if (sem_unlink(SEM_TABLE_NAME) < 0)
        //     syserr("sem_unlink table", __FILE__, __func__, __LINE__);
    }
    if (shm_table_ptr != NULL) 
    {
        if (munmap(shm_table_ptr, (PZ_TABLE_SIZE + 2) * sizeof(int)) < 0)
            syserr("munmap table", __FILE__, __func__, __LINE__);

        // if (shm_unlink(SHM_TABLE_NAME) < 0)
        //     syserr("shm_unlink table", __FILE__, __func__, __LINE__);
    }
}

void logger(char msg[])
{
    static struct timespec ts_curtime;
    clock_gettime(CLOCK_REALTIME, &ts_curtime);
    printf("%d:(%ld:%ld):delivery: %s\n", getpid(), ts_curtime.tv_sec, ts_curtime.tv_nsec / CLOCK_CONV, msg);
}