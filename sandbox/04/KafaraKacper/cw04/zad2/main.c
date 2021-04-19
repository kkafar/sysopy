#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <wait.h>


void custom_sigaction(int, siginfo_t *, void *);
void errsys(const char mssg[]);


int main(int argc, char * argv[]) {
    if (argc != 2) {
        fprintf(stderr, "%s: %d: bad arg count; usage: ./main siginfo|nocldstop|3rd\n", __func__, __LINE__);
        exit(EXIT_FAILURE);
    }

    printf("%s: %d: pid: %d: <-- id of main process\n", __func__, __LINE__, getpid());

    if (strcmp("siginfo", argv[1]) == 0) {
        const int ARBITRARY_SIGNAL = SIGUSR1;
        struct sigaction sigact;
        sigact.sa_sigaction = custom_sigaction;
        sigact.sa_flags = SA_SIGINFO;

        if (sigemptyset(&sigact.sa_mask) < 0) errsys("sigemptyset");

        if (sigaction(ARBITRARY_SIGNAL, &sigact, NULL) < 0) errsys("sigaction");


        // tworzymy proces potomny 
        int cpid;

        if ((cpid = fork()) < 0) {
            errsys("fork");
        } else if (cpid == 0) {
            // czekamy na sygnal
            pause();
            exit(EXIT_SUCCESS);
        } else {
            // odczekujemy 1s, aby miec pewnosc ze juz nastapilo wywolanie pause w potomku 
            sleep(1);
            printf("%s: %d: pid: %d: parent process: sending signal no %d to process %d\n", __func__, __LINE__, getpid(), ARBITRARY_SIGNAL, cpid);
            kill(cpid, ARBITRARY_SIGNAL);
        }

        // wysylamy sygnal do samego siebie 
        printf("%s: %d: pid: %d: parent process: sending signal no %d to process %d\n", __func__, __LINE__, getpid(), ARBITRARY_SIGNAL, cpid);
        kill(getpid(), ARBITRARY_SIGNAL);
    } else if (strcmp("nocldstop", argv[1]) == 0) {
        struct sigaction action;
        action.sa_flags = SA_NOCLDSTOP | SA_SIGINFO;
        action.sa_sigaction = custom_sigaction;

        if (sigemptyset(&action.sa_mask) < 0) errsys("sigemptyset");

        if (sigaction(SIGCHLD, &action, NULL) < 0) errsys("sigaction");

        // if (sigaction(SIGTSTP, &action, NULL) < 0) errsys("sigacion");
        
        // if (sigaction(SIGCONT, &action, NULL) < 0) errsys("sigacion");
        
        int cpid;
        if ((cpid = fork()) < 0) {
            errsys("fork");
        } else if (cpid == 0) {
            pause();
            pause();
            exit(EXIT_SUCCESS);
        } else {
            sleep(2);
            printf("sending sigstop\n");
            kill(cpid, SIGSTOP); 
            sleep(2);   // WYSYLAJAC ZESTAW PRZEZ PRZYPADEK ZAKOMENTOWALEM TA LINIE  --
                        // A JEST ONA KLUCZOWA!!!!! 
            // printf("sending sigcont\n");
            // kill(cpid, SIGCONT);
        }

        // waitpid(cpid, NULL, 0);

        action.sa_flags = SA_SIGINFO;
        // if (sigemptyset(&action.sa_mask) < 0) errsys("sigemptyset");

        if (sigaction(SIGCHLD, &action, NULL) < 0) errsys("sigaction");

        // if (sigaction(SIGTSTP, &action, NULL) < 0) errsys("sigaction");
        
        // if (sigaction(SIGCONT, &action, NULL) < 0) errsys("sigaction");
        
        if ((cpid = fork()) < 0) {
            errsys("fork");
        } else if (cpid == 0) {
            pause();
            pause();
            exit(EXIT_SUCCESS);
        } else {
            sleep(2);
            printf("sending sigstop\n");
            kill(cpid, SIGSTOP); 
            sleep(2);
            // printf("sending sigcont\n");
            // kill(cpid, SIGCONT);
        }
        // waitpid(cpid, NULL, 0);

    } else if (strcmp(argv[1], "resethand") == 0) {
        struct sigaction action;
        action.sa_flags = SA_RESETHAND | SA_SIGINFO;
        action.sa_sigaction = custom_sigaction;

        if (sigemptyset(&action.sa_mask) < 0) errsys("sigemptyset");

        if (sigaction(SIGUSR2, &action, NULL) < 0) errsys("sigaction");

        raise(SIGUSR2);
        raise(SIGUSR2);
    } else {
        fprintf(stderr, "%s: %d: unrecognized argument; ussage: ./main siginfo|nocldstop|3rd\n", __func__, __LINE__);
        exit(EXIT_FAILURE);
    }


    exit(EXIT_SUCCESS);
}


void custom_sigaction(int signo, siginfo_t * info, void * ucontext) {
    if (signo == SIGUSR2) {
        printf("%s: %d: pid: %d: Custom handler for SIGUSR2 executed! SIGNO: %d PPID: %d\n", __func__, __LINE__, getpid(), signo, info->si_pid);
        
    } else {
        printf("%s: %d: pid: %d: Custom handler executed! SIGNO: %d PPID: %d\n", __func__, __LINE__, getpid(), signo, info->si_pid);
    }
}

void errsys(const char mssg[])
{
    int errnum = errno;
    perror(mssg);
    exit(errnum);
}