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
#include <unistd.h>


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

    exit(EXIT_SUCCESS);
}