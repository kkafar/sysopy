#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, char * argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Bad arg count!\n");
        exit(-1);
    }

    long n_process = strtol(argv[1], NULL, 10);
    pid_t wpid;
    // int status;

    if (n_process <= 0) {
        fprintf(stderr, "Invalid number of processes. Must be >= 0.\n");
        exit(-1);
    }

    pid_t children_pids[n_process];

    printf("Parent process id: %d\n", getpid());

    for (long i = 0; i < n_process; ++i) {
        if ((children_pids[i] = fork()) < 0) {
            fprintf(stderr, "Failed to fork process no. %ld\n", i+1);
        } 
        if (children_pids[i] == 0) {
            printf("process: %d, parent: %d\n", getpid(), getppid());
            exit(EXIT_SUCCESS);
        } 

        if ((wpid = waitpid(children_pids[i], NULL, 0)) < 0) {
            int errnum = errno;
            fprintf(stderr, "%d, Failed in waitpid for %ld (%d) process. errno: %d, %s\n", 
                __LINE__, i, children_pids[i], errnum, strerror(errnum));
        }
    }

    return 0;
}