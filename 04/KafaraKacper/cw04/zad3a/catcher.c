#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <wait.h>
#include <stdbool.h>


int no_signals_received = 0;
int SIGNAL1 = SIGUSR1, SIGNAL2 = SIGUSR2;



void handle_sig1(int signo, siginfo_t * info, void * ucontext);
void handle_sig2_kill(int signo, siginfo_t * info, void * ucontext);
void handle_sig2_sigqueue(int signo, siginfo_t * info, void * ucontext);
void handle_sig2_sigrt(int signo, siginfo_t * info, void * ucontext);

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

    struct sigaction action_sig1, action_sig2;
    action_sig1.sa_mask = action_sig2.sa_mask = mask;
    action_sig1.sa_flags = action_sig2.sa_flags = SA_SIGINFO;

    action_sig1.sa_sigaction = handle_sig1;

    /* setting masks && signals handlers */
    if (strcmp("KILL", argv[1]) == 0) 
    {
        if (sigdelset(&mask, SIGNAL1) < 0 || sigdelset(&mask, SIGNAL2) < 0)
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
        action_sig2.sa_sigaction = handle_sig2_kill;
    }
    else if (strcmp("SIGQUEUE", argv[1]) == 0) 
    {
        if (sigdelset(&mask, SIGNAL1) < 0 || sigdelset(&mask, SIGNAL2) < 0)
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
        action_sig2.sa_sigaction = handle_sig2_sigqueue;
    }
    else if (strcmp("SIGRT", argv[1]) == 0) 
    {
        SIGNAL1 = SIGRTMIN;
        SIGNAL2 = SIGRTMIN + 1;
        if (sigdelset(&mask, SIGNAL1) < 0 || sigdelset(&mask, SIGNAL2) < 0)
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
        action_sig2.sa_sigaction = handle_sig2_sigrt;
    }
    else
    {
        fprintf(stderr, "unrecognized mode %s; usage ./catcher KILL|SIGQUEUE|SIGRT\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    if (sigaction(SIGNAL1, &action_sig1, NULL) < 0 || sigaction(SIGNAL2, &action_sig2, NULL) < 0) 
    {
        int errnum = errno;
        perror("sigprocmask");
        exit(errnum);
    }
    
    /* sending out signals is realized in singal handlers */
    while (true)
        sigsuspend(&mask);

    exit(EXIT_SUCCESS);
}


void handle_sig1(int signo, siginfo_t * info, void * ucontext) 
{
    ++no_signals_received;
}

void handle_sig2_kill(int signo, siginfo_t * info, void * ucontext) 
{
    pid_t ppid = info->si_pid;
    int signals_received = no_signals_received;

    for (int i = 0; i < signals_received; ++i) 
    {
        if (kill(ppid, SIGNAL1) < 0)
        {
            int errnum = errno;
            perror("kill");
            exit(errnum);
        }
    }

    if (kill(ppid, SIGNAL2) < 0)
    {
        int errnum = errno;
        perror("kill");
        exit(errnum);
    }

    printf("catcher received %d SIGUSR1(%d) signals\n", signals_received, signo);
    exit(EXIT_SUCCESS);
}

void handle_sig2_sigqueue(int signo, siginfo_t * info, void * ucontext)
{
    pid_t ppid = info->si_pid;
    int signals_received = no_signals_received;
    union sigval value;

    for (int i = 0; i < signals_received; ++i) 
    {
        value.sival_int = i+1;
        if (sigqueue(ppid, SIGNAL1, value) < 0)
        {
            int errnum = errno;
            perror("sigqueue");
            exit(errnum);
        }
    }

    if (sigqueue(ppid, SIGNAL2, value) < 0)
    {
        int errnum = errno;
        perror("sigqueue");
        exit(errnum);
    }

    printf("catcher received %d SIG1(%d) signals\n", signals_received, signo);
    exit(EXIT_SUCCESS);
}

void handle_sig2_sigrt(int signo, siginfo_t * info, void * ucontext)
{
    pid_t ppid = info->si_pid;
    int signals_received = no_signals_received;

    for (int i = 0; i < signals_received; ++i) 
    {
        if (kill(ppid, SIGNAL1) < 0)
        {
            int errnum = errno;
            perror("kill");
            exit(errnum);
        }
    }

    if (kill(ppid, SIGNAL2) < 0)
    {
        int errnum = errno;
        perror("kill");
        exit(errnum);
    }

    printf("catcher received %d SIG1(%d) signals\n", signals_received, signo);
    exit(EXIT_SUCCESS);  
}
