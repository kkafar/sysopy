#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <wait.h>


int main(int argc,  char * argv[]) {
    if (argc != 2) {
        fprintf(stderr, "bad arg count\n");
        exit(EXIT_FAILURE);
    }


    if (strcmp("ignore", argv[1]) == 0) {
        /**
         * Send singal to itself and print if it was ignored 
         * (process will terminate otherwise!) 
         */

        if (raise(SIGUSR1) != 0) {
            fprintf(stderr, "%s: %s: %d: child process failed to send signal to itself\n", __FILE__, __func__, __LINE__);
            exit(EXIT_FAILURE);
        }

        printf("%s: %s: %d: child process did not terminate after sending singal ==> SIGUSR1 is ignored\n", __FILE__, __func__, __LINE__);

    } else if (strcmp("mask", argv[1]) == 0) {
            // is SIGUSR1 masked in child process?
            sigset_t empty_mask, cmask;
            
            if (sigemptyset(&empty_mask) < 0) {
                int errnum = errno;
                fprintf(stderr, "%s: %s: %d: failed to clear sigset, %s\n", __FILE__, __func__, __LINE__, strerror(errnum));
                exit(errnum);
            }

            // this operation does not change mask value!
            // acquiring old mask
            if (sigprocmask(SIG_BLOCK, &empty_mask, &cmask) < 0) {
                int errnum = errno;
                fprintf(stderr, "%s: %s: %d: failed to aquire old sigmask, %s\n", __FILE__, __func__, __LINE__, strerror(errnum));
                exit(errnum);            
            }

            if (sigismember(&cmask, SIGUSR1) == 1) {
                printf("%s: %s: %d: child process: mask is preserved\n", __FILE__, __func__, __LINE__);
                exit(EXIT_SUCCESS);
            } else {
                printf("%s: %s: %d: child process: mask is NOT preserved\n", __FILE__, __func__, __LINE__);
                exit(EXIT_SUCCESS);
            }

    } else if (strcmp("pending", argv[1]) == 0) {
            sigset_t cmask;
            
            if (sigemptyset(&cmask) < 0) {
                int errnum = errno;
                fprintf(stderr, "%s: %s: %d: failed to clear sigset, %s\n", __FILE__, __func__, __LINE__, strerror(errnum));
                exit(errnum);
            }

            if (sigpending(&cmask) < 0) {
                int errnum = errno;
                fprintf(stderr, "%s: %s: %d: %s\n", __FILE__, __func__, __LINE__, strerror(errnum));
                exit(errnum);            
            }

            if (sigismember(&cmask, SIGUSR1) == 1) {
                // printf("pid: %d: child process: signal is pending\n", getpid());
                printf("%s: %s: %d: child process: signal is pending\n", __FILE__, __func__ ,__LINE__);
                exit(EXIT_SUCCESS);
            } else {
                // printf("pid: %d: child process: singal is NOT pending\n", getpid());
                printf("%s: %s: %d: child process: signal is NOT pending\n", __FILE__, __func__ ,__LINE__);
                exit(EXIT_SUCCESS);
            }

    } else {
        fprintf(stderr, "unrecognized argument; usage ./mainexec [ignore|mask|pending] -- exactly one of\n");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS); 
}