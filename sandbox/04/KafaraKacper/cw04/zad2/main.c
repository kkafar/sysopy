/** 
 * Program testuje dzialanie flag:
 * SA_SIGINFO
 * SA_NOCLDSTOP (tworzymy dziecko, ktore wykonuje sie jakis ustalony czas, wysylamy do niego sygnal 
 * zatrzymujacy, po czym sprawdzamy czy proces macierzysty otrzymal suygnal SIGCHLD)
 * 3rd
 * w funkcji sigaction.
 * 
 * Dla flagi SA_SIGINFO:
 * Ustawiamy handler dla wybranego sygnału (wybarano SIGUSR1) z tą flagą. 
 * Pokazujemy, że wykonywany jest sa_sigaction a nie sa_handler, np. forkując i wysyłając
 * ten sygnał do procesu potomnego, który sygnał obsłuży wypisując komunikat oraz informacje 
 * nt procesu ktory sygnal nadal.
 * 
 * Dla flagi SA_NOCLDSTOP 
 * 
 */


#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>


void custom_sigaction(int, siginfo_t *, void *);


int main(int argc, char * argv[]) {


    printf("%s: %d: pid: %d: <-- id of main process\n", __func__, __LINE__, getpid());
    

    
    // SA_NOCLDSTOP

    
    if (argc != 2) {
        fprintf(stderr, "%s: %d: bad arg count; usage: ./main siginfo|nocldstop|3rd\n", __func__, __LINE__);
        exit(EXIT_FAILURE);
    }

    if (strcmp("siginfo", argv[1]) == 0) {
        const int ARBITRARY_SIGNAL = SIGUSR1;
        siginfo_t info;
        struct sigaction sigact;
        sigact.sa_sigaction = custom_sigaction;
        sigact.sa_flags = SA_SIGINFO;

        if (sigaction(ARBITRARY_SIGNAL, &sigact, NULL) < 0) {
            int errnum = errno;
            perror("sigaction");
            exit(errnum);
        }

        // tworzymy proces potomny 
        int cpid;

        if ((cpid = fork()) < 0) {
            int errnum = errno;
            perror("fork");
            exit(errnum);
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
        struct sigaction sigact;
        


    } else {
        fprintf(stderr, "%s: %d: unrecognized argument; ussage: ./main siginfo|nocldstop|3rd\n", __func__, __LINE__);
        exit(EXIT_FAILURE);
    }


    exit(EXIT_SUCCESS);
}


void custom_sigaction(int signo, siginfo_t * info, void * ucontext) {
    printf("%s: %d: pid: %d: Custom handler executed! SIGNO: %d PPID: %d\n", __func__, __LINE__, getpid(), signo, info->si_pid);
    exit(EXIT_SUCCESS);
}
