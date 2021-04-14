#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <wait.h>


void handler(int);


int main(int argc, char * argv[]) {
    if (argc != 2) {
        fprintf(stderr, "bad arg count\n");
        exit(EXIT_FAILURE);
    }

    if (strcmp("mask", argv[1]) == 0) {
        sigset_t sigset;

        if (sigemptyset(&sigset) < 0) {
            int errnum = errno;
            fprintf(stderr, "%s: %s: %d: failed to clear sigset, %s\n", __FILE__, __func__, __LINE__, strerror(errnum));
            exit(errnum);
        }

        if (sigaddset(&sigset, SIGUSR1) < 0) {
            int errnum = errno;
            fprintf(stderr, "%s: %s: %d: failed to add SIGUSR1 to sigset, %s\n", __FILE__, __func__, __LINE__, strerror(errnum));
            exit(errnum);            
        }

        // setting mask for current process
        if (sigprocmask(SIG_BLOCK, &sigset, NULL) < 0) {
            int errnum = errno;
            fprintf(stderr, "%s: %s: %d: failed to set new sigmask, %s\n", __FILE__, __func__, __LINE__, strerror(errnum));
            exit(errnum);            
        }

        // process sends singal to itself
        if (raise(SIGUSR1) != 0) {
            fprintf(stderr, "%s: %s: %d: parent process failed to send signal to itself\n", __FILE__, __func__, __LINE__);
            exit(EXIT_FAILURE);
        }

        char * args[] = {"./toexec", "mask", NULL};
        if (execv("toexec", args) < 0) {
            int errnum = errno;
            fprintf(stderr, "%s: %s: %d: %s\n", __FILE__, __func__, __LINE__, strerror(errnum));
            exit(errnum);
        }


    } else if (strcmp("ignore", argv[1]) == 0) {
        signal(SIGUSR1, SIG_IGN);

        if (raise(SIGUSR1) != 0) {
            fprintf(stderr, "%s: %s: %d: parent process failed to send signal to itself\n", __FILE__, __func__, __LINE__);
            exit(EXIT_FAILURE);
        }

        printf("%s: %s: %d: parent process did not terminate after sending singal ==> SIGUSR1 is ignored\n", __FILE__, __func__, __LINE__);

        char * args[] = {"./toexec", "ignore", NULL};
        if (execv("toexec", args) < 0) {
            int errnum = errno;
            fprintf(stderr, "%s: %s: %d: %s\n",__FILE__, __func__, __LINE__, strerror(errnum));
            exit(errnum);
        }
    } else if (strcmp("pending", argv[1]) == 0) {
        sigset_t sigset, empty_set;

        if (sigemptyset(&sigset) < 0 || sigemptyset(&empty_set) < 0) {
            int errnum = errno;
            fprintf(stderr, "%s: %s: %d: failed to clear sigset | empty_set, %s\n", __FILE__, __func__, __LINE__, strerror(errnum));
            exit(errnum);
        }

        if (sigaddset(&sigset, SIGUSR1) < 0) {
            int errnum = errno;
            fprintf(stderr, "%s: %s: %d: failed to add SIGUSR1 to sigset, %s\n", __FILE__, __func__, __LINE__, strerror(errnum));
            exit(errnum);            
        }

        if (sigprocmask(SIG_BLOCK, &sigset, NULL) < 0) {
            int errnum = errno;
            fprintf(stderr, "%s: %s: %d: failed to set new sigmask, %s\n", __FILE__, __func__, __LINE__, strerror(errnum));
            exit(errnum);            
        }

        if (raise(SIGUSR1) != 0) {
            fprintf(stderr, "%s: %d: parent process failed to send signal to itself\n", __func__, __LINE__);
            exit(EXIT_FAILURE);
        }

        if (sigpending(&empty_set) < 0) {
            int errnum = errno;
            fprintf(stderr, "%s: %s: %d: %s\n", __FILE__, __func__, __LINE__, strerror(errnum));
            exit(errnum);            
        }

        if (sigismember(&empty_set, SIGUSR1) == 1) {
            printf("%s: %s: %d: parent process: signal is pending\n", __FILE__, __func__ ,__LINE__);
        }

        char * args[] = {"./toexec", "pending", NULL};
        if (execv("toexec", args) < 0) {
            int errnum = errno;
            fprintf(stderr, "%s: %d: %s\n", __func__, __LINE__, strerror(errnum));
            exit(errnum);
        }

    } else {
        fprintf(stderr, "unrecognized argument; usage ./main [ignore|handler|mask|pending] -- exactly one of\n");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}

void handler(int signo) {
    printf("%s: %s: %d: recieved SIGUSR1 (%d)\n", __FILE__, __func__, __LINE__, signo);
}