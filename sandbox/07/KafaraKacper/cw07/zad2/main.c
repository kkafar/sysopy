#include <stdlib.h>
#include <stdio.h>
#include "util.h"
#include "constants.h"
#include <semaphore.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include <wait.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>


void launch_cook();
void launch_delivery();
void handle_sigint(int signo);
void cleanup();
void generate_string(char * buf, size_t bufsize);


sem_t * sem_owen_ptr = NULL;
sem_t * sem_table_ptr = NULL;
int * shm_owen_ptr = NULL;
int * shm_table_ptr = NULL;
int fd_shm_owen = -1;
int fd_shm_table = -1;



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

    errno = 0; 
    /* tworzenie semaforów */
    if ((sem_owen_ptr = sem_open(SEM_OWEN_NAME, O_CREAT | O_EXCL | O_RDWR, 0666, 1)) == SEM_FAILED)
        syserr("sem_open owen", __FILE__, __func__, __LINE__);

    if ((sem_table_ptr = sem_open(SEM_TABLE_NAME, O_CREAT | O_EXCL | O_RDWR, 0666, 1)) == SEM_FAILED)
        syserr("sem_open table", __FILE__, __func__, __LINE__);

    /* rezerwacja pamięci współdzielonej */
    if ((fd_shm_owen = shm_open(SHM_OWEN_NAME, O_CREAT | O_EXCL | O_RDWR, 0666)) < 0) 
        syserr("shm_open owen", __FILE__, __func__, __LINE__);

    /* rezerwacja obszaru */
    printf("length value passed to ftruncate %ld\n", (PZ_OWEN_SIZE + 1) * sizeof(int));
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


    if ((fd_shm_table = shm_open(SHM_TABLE_NAME, O_CREAT | O_EXCL | O_RDWR, 0666)) < 0)
        syserr("shm_open table", __FILE__, __func__, __LINE__);

    /* rezerwacja obszaru */
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

    /* incijalizujemy pamięć */
    for (size_t i = 0; i < PZ_OWEN_SIZE; ++i) shm_owen_ptr[i] = -1;
    for (size_t i = 0; i < PZ_TABLE_SIZE; ++i) shm_table_ptr[i] = -1;
    shm_owen_ptr[PZ_OWEN_SIZE] = PZ_OWEN_SIZE;
    shm_table_ptr[PZ_TABLE_SIZE] = PZ_TABLE_SIZE; // liczba wolnych miejsc na stole
    shm_table_ptr[PZ_TABLE_SIZE + 1] = N * PZ_NUM; // pizze pozostale do przetworzenia

    /* tworzymy kucharzy */
    for (int i = 0; i < N; ++i)
        launch_cook();

    // /* tworzymy dostawców */
    // for (int i = 0; i < M; ++i)
    //     launch_delivery();

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
    if (shm_owen_ptr != NULL) 
    {
        if (munmap(shm_owen_ptr, (PZ_OWEN_SIZE + 1) * sizeof(int)) < 0)
            syserr("munmap owen", __FILE__, __func__, __LINE__);

        if (shm_unlink(SHM_OWEN_NAME) < 0)
            syserr("shm_unlink owen", __FILE__, __func__, __LINE__);
    }
    if (shm_table_ptr != NULL) 
    {
        if (munmap(shm_table_ptr, (PZ_TABLE_SIZE + 2) * sizeof(int)) < 0)
            syserr("munmap table", __FILE__, __func__, __LINE__);

        if (shm_unlink(SHM_TABLE_NAME) < 0)
            syserr("shm_unlink table", __FILE__, __func__, __LINE__);
    }
}