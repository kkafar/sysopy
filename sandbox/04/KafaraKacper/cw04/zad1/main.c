#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <wait.h>


/**
 * ignore: 
 * w procesie przodka ustawia ignorowanie SIGUSR1
 * 
 * handler:
 * w procesie przeodka ustawia handler do łapania i obsługi
 * sygnału SIGUSR1 (wypisuje komunikat o otrzymaniu sygnału);
 * 
 * mask: 
 * w procesie przodka maskuje sygnał SIGUSR1
 * 
 * pending: 
 * w procesie przodka maskuje sygnał SIGUSR1 oraz 
 * sprawdza czy oczekujący sygnał jest widoczny w procesie
 * 
 * dalej dla każdej z opcji:
 * wysyła sygnał do samego siebie;
 * tworzy potomka;
 * potomek wysyła sygnał do samego siebie (przy pending 
 * sprawdzamy tylko czy sygnał wysłany w przodku jest widoczny w potomku)
 * 
 * Ewentualnie wykonać jeszcze jeden program do sprawdzania większej ilości sygnałów. 
 */

void handler(int);


int main(int argc, char * argv[]) {
    if (argc != 2) {
        fprintf(stderr, "bad arg count\n");
        exit(EXIT_FAILURE);
    }

    if (strcmp("mask", argv[1]) == 0) {
        sigset_t sigset;

        if (sigemptyset(&sigset) < 0) {
            // może to byc potencjalnie błędny komunikat 
            int errnum = errno;
            fprintf(stderr, "%s: %d: failed to clear sigset, %s\n", __func__, __LINE__, strerror(errnum));
            exit(errnum);
        }

        if (sigaddset(&sigset, SIGUSR1) < 0) {
            int errnum = errno;
            fprintf(stderr, "%s: %d: failed to add SIGUSR1 to sigset, %s\n", __func__, __LINE__, strerror(errnum));
            exit(errnum);            
        }

        // ustawiamy maskę dla aktualnego procesu 
        if (sigprocmask(SIG_BLOCK, &sigset, NULL) < 0) {
            int errnum = errno;
            fprintf(stderr, "%s: %d: failed to set new sigmask, %s\n", __func__, __LINE__, strerror(errnum));
            exit(errnum);            
        }

        // wysyłamy sygnał do samego siebie
        if (raise(SIGUSR1) != 0) {
            fprintf(stderr, "%s: %d: parent process failed to send signal to itself\n", __func__, __LINE__);
            exit(EXIT_FAILURE);
        }

        int cpid;

        if ((cpid = fork()) < 0) {
            int errnum = errno;
            fprintf(stderr, "%s: %d: %s\n", __func__, __LINE__, strerror(errnum));
            exit(errnum);
        } else if (cpid == 0) {
            // sprawdzamy czy SIGUSR1 jest zamaskowany w potomku
            sigset_t empty_mask, cmask;
            
            if (sigemptyset(&empty_mask) < 0) {
                // może to byc potencjalnie błędny komunikat 
                int errnum = errno;
                fprintf(stderr, "%s: %d: failed to clear sigset, %s\n", __func__, __LINE__, strerror(errnum));
                exit(errnum);
            }

            // ta operacja nie zmienia wartości maski! 
            // pozyskujemy starą maskę
            if (sigprocmask(SIG_BLOCK, &empty_mask, &cmask) < 0) {
                int errnum = errno;
                fprintf(stderr, "%s: %d: failed to set new sigmask, %s\n", __func__, __LINE__, strerror(errnum));
                exit(errnum);            
            }

            if (sigismember(&cmask, SIGUSR1) == 1) {
                printf("pid: %d: child process: mask is preserved\n", getpid());
                exit(EXIT_SUCCESS);
            } else {
                printf("pid: %d: child process: mask is NOT preserved\n", getpid());
                exit(EXIT_SUCCESS);
            }

        } else {
            waitpid(cpid, NULL, 0);
        }
    } else if (strcmp("handler", argv[1]) == 0) {
        struct sigaction action;

        if (sigemptyset(&action.sa_mask) < 0) {
            // może to byc potencjalnie błędny komunikat 
            int errnum = errno;
            fprintf(stderr, "%s: %d: failed to clear sigset, %s\n", __func__, __LINE__, strerror(errnum));
            exit(errnum);
        }        

        action.sa_handler = handler;
        action.sa_flags = 0;

        if (sigaction(SIGUSR1, &action, NULL) < 0) {
            int errnum = errno;
            fprintf(stderr, "%s: %d: failed to clear sigset, %s\n", __func__, __LINE__, strerror(errnum));
            exit(errnum);            
        }

        // wysyłamy sygnał do samego siebie
        if (raise(SIGUSR1) != 0) {
            fprintf(stderr, "%s: %d: parent process failed to send signal to itself\n", __func__, __LINE__);
            exit(EXIT_FAILURE);
        }

        int cpid;

        if ((cpid = fork()) < 0) {
            int errnum = errno;
            fprintf(stderr, "%s: %d: %s\n", __func__, __LINE__, strerror(errnum));
            exit(errnum);
        } else if (cpid == 0) {
            // wysyłamy sygnał do samego siebie
            if (raise(SIGUSR1) != 0) {
                fprintf(stderr, "%s: %d: child process failed to send signal to itself\n", __func__, __LINE__);
                exit(EXIT_FAILURE);
            }

        } else {
            waitpid(cpid, NULL, 0);
        }

    } else if (strcmp("ignore", argv[1]) == 0) {
        signal(SIGUSR1, SIG_IGN);

        if (raise(SIGUSR1) != 0) {
            fprintf(stderr, "%s: %d: parent process failed to send signal to itself\n", __func__, __LINE__);
            exit(EXIT_FAILURE);
        }

        printf("%s: %d: parent process did not terminate after sending singal ==> SIGUSR1 is ignored\n", __func__, __LINE__);

        int cpid;

        if ((cpid = fork()) < 0) {
            int errnum = errno;
            fprintf(stderr, "%s: %d: %s\n", __func__, __LINE__, strerror(errnum));
            exit(errnum);
        } else if (cpid == 0) {
            // wysyłamy sygnał do samego siebie
            if (raise(SIGUSR1) != 0) {
                fprintf(stderr, "%s: %d: child process failed to send signal to itself\n", __func__, __LINE__);
                exit(EXIT_FAILURE);
            }
            printf("%s: %d: child process did not terminate after sending singal ==> SIGUSR1 is ignored\n", __func__, __LINE__);

        } else {
            waitpid(cpid, NULL, 0);
        }

    } else if (strcmp("pending", argv[1]) == 0) {
        sigset_t sigset, empty_set;

        if (sigemptyset(&sigset) < 0 || sigemptyset(&empty_set) < 0) {
            // może to byc potencjalnie błędny komunikat 
            int errnum = errno;
            fprintf(stderr, "%s: %d: failed to clear sigset | empty_set, %s\n", __func__, __LINE__, strerror(errnum));
            exit(errnum);
        }

        if (sigaddset(&sigset, SIGUSR1) < 0) {
            int errnum = errno;
            fprintf(stderr, "%s: %d: failed to add SIGUSR1 to sigset, %s\n", __func__, __LINE__, strerror(errnum));
            exit(errnum);            
        }

        // ustawiamy maskę dla aktualnego procesu 
        if (sigprocmask(SIG_BLOCK, &sigset, NULL) < 0) {
            int errnum = errno;
            fprintf(stderr, "%s: %d: failed to set new sigmask, %s\n", __func__, __LINE__, strerror(errnum));
            exit(errnum);            
        }

        // wysyłamy sygnał do samego siebie
        if (raise(SIGUSR1) != 0) {
            fprintf(stderr, "%s: %d: parent process failed to send signal to itself\n", __func__, __LINE__);
            exit(EXIT_FAILURE);
        }

        // sprawdzamy czy sygnał ma status pending 
        if (sigpending(&empty_set) < 0) {
            int errnum = errno;
            fprintf(stderr, "%s: %d: %s\n", __func__, __LINE__, strerror(errnum));
            exit(errnum);            
        }

        if (sigismember(&empty_set, SIGUSR1) == 1) {
            printf("%s: %d: parent process: signal is pending\n", __func__ ,__LINE__);
        }


        int cpid;

        if ((cpid = fork()) < 0) {
            int errnum = errno;
            fprintf(stderr, "%s: %d: %s\n", __func__, __LINE__, strerror(errnum));
            exit(errnum);
        } else if (cpid == 0) {
            // sprawdzamy czy SIGUSR1 jest zamaskowany w potomku
            sigset_t cmask;
            
            if (sigemptyset(&cmask) < 0) {
                // może to byc potencjalnie błędny komunikat 
                int errnum = errno;
                fprintf(stderr, "%s: %d: failed to clear sigset, %s\n", __func__, __LINE__, strerror(errnum));
                exit(errnum);
            }

            if (sigismember(&cmask, SIGUSR1) == 1) {
                printf("pid: %d: child process: signal is pending\n", getpid());
                exit(EXIT_SUCCESS);
            } else {
                printf("pid: %d: child process: singal is NOT pending\n", getpid());
                exit(EXIT_SUCCESS);
            }

        } else {
            waitpid(cpid, NULL, 0);
        }

    } else {
        fprintf(stderr, "unrecognized argument; usage ./main [ignore|handler|mask|pending] -- exactly one of\n");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}

void handler(int signo) {
    printf("%d: %s: %d: recieved SIGUSR1 (%d)\n", getpid(), __func__, __LINE__, signo);
}