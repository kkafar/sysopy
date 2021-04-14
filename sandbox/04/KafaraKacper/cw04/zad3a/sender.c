#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <wait.h>
#include <stdbool.h>


int no_received_signals = 0;
int no_signals;

void handler_sigusr1(int signo);
void handler_sigusr2(int signo);


int main(int argc, char * argv[]) 
{
    /* checking for right number of arguments */
    if (argc != 4) 
    {
        fprintf(stderr, "%s: %d: bad arg count; usage ./sender CATCHER_PID NO_SIGNALS KILL|SIGQUEUE|SIGRT\n", __func__, __LINE__);
        exit(EXIT_FAILURE);
    }

    pid_t catcher_pid;

    /* some basic chceck for pid validity */
    if ((catcher_pid = strtol(argv[1], NULL, 10)) < 0) 
    {
        fprintf(stderr, "%s: %d: non-positive catcher pid\n", __func__, __LINE__);
        exit(EXIT_FAILURE);
    }
    
    /* check if no_signals > 0 */
    if ((no_signals = strtol(argv[2], NULL, 10)) <= 0) 
    {
        fprintf(stderr, "%s: %d: non-positive number of signals\n", __func__, __LINE__);
        exit(EXIT_FAILURE);
    }

    /* KILL mode */
    if (strcmp(argv[3], "KILL") == 0)
    {
        /* masking all signals except SIGUSR1 && SIGUSR2 */
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
        

        struct sigaction action_usr1, action_usr2;
        action_usr1.sa_handler = handler_sigusr1;
        action_usr2.sa_handler = handler_sigusr2;
        
        if (sigemptyset(&action_usr1.sa_mask) < 0 || sigemptyset(&action_usr2.sa_mask) < 0)
        {
            int errnum = errno;
            perror("sigemptyset");
            exit(errnum);
        }

        if (sigaction(SIGUSR1, &action_usr1, NULL) < 0 || sigaction(SIGUSR2, &action_usr2, NULL) < 0) 
        {
            int errnum = errno;
            perror("sigaction");
            exit(errnum);
        }

        for (int i = 0; i < no_signals; ++i) 
        {
            if (kill(catcher_pid, SIGUSR1) < 0) 
            {
                int errnum = errno;
                perror("kill SIGUSR1");
                exit(errnum);
            }
        }

        if (kill(catcher_pid, SIGUSR2) < 0)
        {
            int errnum = errno;
            perror("kill SIGUSR2");
            exit(errnum);
        }

        /* waiting for signals, SIGUSR2 terminates process */
        while (true) 
            sigsuspend(&mask);
    }
    else if (strcmp(argv[3], "SIGQUEUE") == 0) 
    {

    }
    else if (strcmp(argv[3], "SIGRT") == 0) 
    {

    }
    else 
    {
        fprintf(stderr, "%s: %d: unrecognized mode: %s\n", __func__, __LINE__, argv[3]);
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}


void handler_sigusr1(int signo) 
{
    ++no_received_signals;
}


void handler_sigusr2(int signo)
{
    printf("sender received SIGUSR2(%d). %d/%d SIGUSR1 signals were fetched\n", signo, no_received_signals, no_signals);
    exit(EXIT_SUCCESS);
}