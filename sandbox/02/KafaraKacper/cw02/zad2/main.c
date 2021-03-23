#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>


#define MAX_LINE 256

#ifdef LIB
const char * TYPE = "LIB";
#endif

#ifdef SYS
const char * TYPE = "SYS";
#endif


#ifdef BENCHMARK
void compute_elapsed_time(struct timespec * start, struct timespec * stop, struct timespec * elapsed);
void make_report(struct timespec * start, struct timespec * stop);
#endif

int main(int argc, char * argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Invalid number of aruments: %d. Expected 2\n", argc - 1);
        exit(1);
    }

    char ch             = argv[1][0];
    char * pathname     = argv[2];

    char buf[MAX_LINE];
    int i = 0;
    bool printflag = false;

#ifdef BENCHMARK
    struct timespec tp_start, tp_stop;
    clock_gettime(CLOCK_REALTIME, &tp_start);
#endif

#ifdef LIB
    FILE * fp  = fopen(pathname, "r");

    if (!fp) {
        fprintf(stderr, "errno: %d, %s\n", errno, strerror(errno));
        fclose(fp);
        exit(1);
    }

    while (fread(buf + i, sizeof(char), 1, fp)) {
        if (buf[i] == ch) printflag = true;
        
        if (buf[i] == '\n') {
            if (printflag) {
                for (int j = 0; j <= i; ++j)
                    fwrite(buf + j, sizeof(char), 1, stdout);

                printflag = false;
            }
            i = -1;
        }
        ++i;
    }   
    if (ferror(fp)) {
        fprintf(stderr, "errno: %d, %s\n", errno, strerror(errno));
        fclose(fp);
        exit(2);
    }

    fclose(fp);
#endif 

#ifdef SYS
    int fd = open(pathname, O_RDONLY);

    if (fd == -1) {
        fprintf(stderr, "errno: %d, %s\n", errno, strerror(errno));
        exit(2);
    }

    int read_ret;

    while ((read_ret = read(fd, buf + i, sizeof(char))) > 0) {
        if (buf[i] == ch) printflag = true;

        if (buf[i] == '\n') {
            if (printflag) {
                for (int j = 0; j <= i; ++j)
                    write(STDOUT_FILENO, buf + j, sizeof(char));

                printflag = false;
            }
            i = -1;
        }
        ++i;
    }

    if (read_ret == -1) {
        fprintf(stderr, "errno: %d, %s\n", errno, strerror(errno));
        close(fd);
        exit(3);
    }

    close(fd);
#endif


#ifdef BENCHMARK
    clock_gettime(CLOCK_REALTIME, &tp_stop);
    make_report(&tp_start, &tp_stop);
#endif

    return 0;
}


#ifdef BENCHMARK
void compute_elapsed_time(struct timespec * start, struct timespec * stop, struct timespec * elapsed) {
    elapsed->tv_sec = stop->tv_sec - start->tv_sec;
    
    elapsed->tv_nsec = stop->tv_nsec - start->tv_nsec;

    if (elapsed->tv_nsec < 0) {
        --elapsed->tv_sec;
        elapsed->tv_nsec += (long)(1E9);
    }
}
#endif


#ifdef BENCHMARK
void make_report(struct timespec * start, struct timespec * stop) {
    struct timespec elapsed;
    compute_elapsed_time(start, stop, &elapsed);

    FILE * report = fopen("pomiar_zad_2.txt", "w");

    if (!report) {
        fprintf(stderr, "%d: errno: %d, %s\n", __LINE__, errno, strerror(errno));
        fclose(report);
        exit(1);
    }

    fprintf(report, "Type: %s, realtime: %ld.%5lds\n", TYPE, elapsed.tv_sec, elapsed.tv_nsec);

    fclose(report);
}
#endif