#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <wait.h>
#include <stdbool.h>


int no_signals_received = 0;

void handle_sigusr1(int signo, siginfo_t * info, void * ucontext);
void handle_sigusr2(int signo, siginfo_t * info, void * ucontext);


int main(int argc, char * argv[]) 
{
    if (argc != 2)
    {
        fprintf(stderr, "%s: %d: bad arg count; usage ./catcher KILL|SIGQUEUE|SIGRT\n", __func__, __LINE__);
        exit(EXIT_FAILURE);
    }

    printf("%s: %d: catcher's pid: %d\n", __func__, __LINE__, getpid());


    // masking all signals except SIGUSR1 && SIGUSR2
    sigset_t mask;

    if (sigfillset(&mask) < 0) 
    {
        int errnum = errno;
        perror("sigfillset");
        exit(errnum);
    }

    if (sigdelset(&mask, SIGUSR1) < 0 || sigdelset(&mask, SIGUSR2) < 0)
    {
        int errnum = errno;
        perror("sigdelset");
        exit(errnum);
    }

    if (sigprocmask(SIG_SETMASK, &mask, NULL) < 0) 
    {
        int errnum = errno;
        perror("sigprocmask");
        exit(errnum);
    }





    exit(EXIT_SUCCESS);
}


void handle_sigusr1(int signo, siginfo_t * info, void * ucontext) 
{
    ++no_signals_received;
}


void handle_sigusr2(int signo, siginfo_t * info, void * ucontext) 
{

}