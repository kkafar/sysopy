#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <wait.h>
#include <stdbool.h>


int SIG1 = SIGUSR1, SIG2 = SIGUSR2;

int no_received_signals = 0;
int no_signals;

void handle_sig1(int signo);
void handle_sig2(int signo);
void handle_sig2_sigqueue(int signo, siginfo_t * info, void * ucontext);
void killmode(pid_t catcher_pid, sigset_t * mask);
void sigqueuemode(pid_t catcher_pid, sigset_t * mask);


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

    /* masking all signals except SIG1 && SIG2 */
    sigset_t mask;
        
    if (sigfillset(&mask) < 0) 
    {
        int errnum = errno;
        perror("sigfillset");
        exit(errnum);
    }

    printf("%s: %d: sender's pid: %d\n", __func__, __LINE__, getpid());
    /* KILL mode */
    if (strcmp(argv[3], "KILL") == 0)
        killmode(catcher_pid, &mask);
    
    else if (strcmp(argv[3], "SIGQUEUE") == 0) 
        sigqueuemode(catcher_pid, &mask);

    else if (strcmp(argv[3], "SIGRT") == 0) 
    {
        /* swapping signals */
        SIG1 = SIGRTMIN;
        SIG2 = SIGRTMIN + 1;
        killmode(catcher_pid, &mask);
    }
    else 
    {
        fprintf(stderr, "%s: %d: unrecognized mode: %s\n", __func__, __LINE__, argv[3]);
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}


void handle_sig1(int signo) 
{
    ++no_received_signals;
}


void handle_sig2(int signo)
{
    printf("sender received SIGUSR2(%d). %d/%d SIGUSR1 signals were fetched back (%.2lf)%%\n", signo, no_received_signals, no_signals, (double)(no_received_signals) / no_signals * 100);
    exit(EXIT_SUCCESS);
}


void killmode(pid_t catcher_pid, sigset_t * mask)
{
    if (sigdelset(mask, SIG1) < 0 || sigdelset(mask, SIG2) < 0) 
    {
        int errnum = errno;
        perror("sigdelset");
        exit(errnum);
    }

    if (sigprocmask(SIG_SETMASK, mask, NULL) < 0)
    {
        int errnum = errno;
        perror("sigprocmask");
        exit(errnum);
    }
    
    struct sigaction action_sig1, action_sig2;
    action_sig1.sa_handler = handle_sig1;
    action_sig2.sa_handler = handle_sig2;
    
    if (sigemptyset(&action_sig1.sa_mask) < 0 || sigemptyset(&action_sig2.sa_mask) < 0)
    {
        int errnum = errno;
        perror("sigemptyset");
        exit(errnum);
    }

    if (sigaction(SIG1, &action_sig1, NULL) < 0 || sigaction(SIG2, &action_sig2, NULL) < 0) 
    {
        int errnum = errno;
        perror("sigaction");
        exit(errnum);
    }

    for (int i = 0; i < no_signals; ++i) 
    {
        if (kill(catcher_pid, SIG1) < 0) 
        {
            int errnum = errno;
            perror("kill SIG1");
            exit(errnum);
        }
    }

    if (kill(catcher_pid, SIG2) < 0)
    {
        int errnum = errno;
        perror("kill SIG2");
        exit(errnum);
    }

    /* waiting for signals, SIG2 terminates process */
    while (true) 
        sigsuspend(mask);
}

void sigqueuemode(pid_t catcher_pid, sigset_t * mask) 
{
    if (sigdelset(mask, SIG1) < 0 || sigdelset(mask, SIG2) < 0) 
    {
        int errnum = errno;
        perror("sigdelset");
        exit(errnum);
    }

    if (sigprocmask(SIG_SETMASK, mask, NULL) < 0)
    {
        int errnum = errno;
        perror("sigprocmask");
        exit(errnum);
    }
    
    struct sigaction action_sig1, action_sig2;
    action_sig1.sa_handler = handle_sig1;
    action_sig2.sa_sigaction = handle_sig2_sigqueue; 
    action_sig2.sa_flags = SA_SIGINFO;

    if (sigemptyset(&action_sig1.sa_mask) < 0 || sigemptyset(&action_sig2.sa_mask) < 0)
    {
        int errnum = errno;
        perror("sigemptyset");
        exit(errnum);
    }

    if (sigaction(SIG1, &action_sig1, NULL) < 0 || sigaction(SIG2, &action_sig2, NULL) < 0)
    {
        int errnum = errno;
        perror("sigaction");
        exit(errnum);
    }

    union sigval sval; 
    sval.sival_int = 0;
    for (int i = 0; i < no_signals; ++i) 
    {
        if (sigqueue(catcher_pid, SIG1, sval) < 0) 
        {
            int errnum = errno;
            perror("sigqueue");
            exit(errnum);
        }
    }

    if (sigqueue(catcher_pid, SIG2, sval) < 0) 
    {
        int errnum = errno;
        perror("sigqueue");
        exit(errnum);
    }

    while (true)
        sigsuspend(mask);
}

void handle_sig2_sigqueue(int signo, siginfo_t * info, void * ucontext)
{
    printf("sender received SIG2(%d). %d/%d SIG1(%d) signals were fetched back (%.2lf)%%. Catcher confirmed %d signals.\n", 
        signo, 
        no_received_signals, 
        no_signals, 
        SIG1, 
        (double)(no_received_signals) / no_signals * 100, 
        info->si_int);
    exit(EXIT_SUCCESS);
}