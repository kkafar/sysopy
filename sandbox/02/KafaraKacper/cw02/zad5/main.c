#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>


#ifdef LIB
const char * TYPE = "LIB";
#endif

#ifdef SYS
const char * TYPE = "SYS";
#endif



char * get_rand_str(int leng);
void compute_elapsed_time(struct timespec * start, struct timespec * stop, struct timespec * elapsed);
void make_report(struct timespec * start, struct timespec * stop);

int main(int argc, char * argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Invalid number of arguments: %d. Expectd 2.\n", argc);
        exit(1);
    }

    char ch;
    int count = 0;

#ifdef BENCHMARK
    struct timespec tp_start, tp_stop;
    clock_gettime(CLOCK_REALTIME, &tp_start);
#endif

#ifdef LIB
    FILE * input_file = fopen(argv[1], "r");
    FILE * output_file = fopen(argv[2], "w");

    if (!input_file || !output_file) {
        fprintf(stderr, "%d: errno: %d, %s\n", __LINE__, errno, strerror(errno));
        exit(1);
    }

    while (fread(&ch, sizeof(char), 1, input_file)) {
        ++count;

        if (ch == '\n') {
            count = 0;
            fwrite(&ch, sizeof(char), 1, output_file);
        } else if (count <= 50) {
            fwrite(&ch, sizeof(char), 1, output_file);
        } else {
            fwrite("\n", sizeof(char), 1, output_file);
            fwrite(&ch, sizeof(char), 1, output_file);
            count = 1;
        }
    }
    if (feof(input_file)) {
        fclose(input_file);
        fclose(output_file);
    } else if (ferror(input_file)) {
        fprintf(stderr, "%d: errno: %d, %s\n", __LINE__, errno, strerror(errno));
        fclose(input_file);
        fclose(output_file);
    }
#endif

#ifdef SYS
    int inputfd, outputfd, retcode;

    inputfd = open(argv[1], O_RDONLY);
    outputfd = open(argv[2], O_WRONLY | O_CREAT, S_IRWXU);

    if (inputfd == -1 || outputfd == -1) {
        fprintf(stderr, "%d: errno: %d, %s\n", __LINE__, errno, strerror(errno));
        exit(1);
    }

    while ((retcode = read(inputfd, &ch, sizeof(char))) > 0) {
        ++count;

        if (ch == '\n') {
            count = 0; 
            write(outputfd, &ch, sizeof(char));
        } else if (count <= 50) {
            write(outputfd, &ch, sizeof(char));
        } else {
            write(outputfd, "\n", sizeof(char));
            write(outputfd, &ch, sizeof(char));
            count = 1;
        }
    }
    if (retcode == 0) {
        close(inputfd);
        close(outputfd);
    } else if (retcode == -1) {
        fprintf(stderr, "%d: errno: %d, %s\n", __LINE__, errno, strerror(errno));
        close(inputfd);
        close(outputfd);
    }
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

    FILE * report = fopen("pomiar_zad_5.txt", "w");

    if (!report) {
        fprintf(stderr, "%d: errno: %d, %s\n", __LINE__, errno, strerror(errno));
        fclose(report);
        exit(1);
    }

    fprintf(report, "Type: %s, realtime: %ld.%5lds\n", TYPE, elapsed.tv_sec, elapsed.tv_nsec);

    fclose(report);
}
#endif



/* Jeden bajt przeznaczony na nulla! Długość zwróconej linii to leng - 1! */
char * get_rand_str(int leng)
{
    if (leng < 2) return NULL;

    static char * sigma = "QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm";
    static int sigmalen = 52;

    char * str = (char *) malloc(sizeof(char) * (leng+1));

    if (!str) return NULL;

    for (int i = 0; i < leng - 1; ++i) str[i] = sigma[rand() % sigmalen];

    str[leng - 1] = '\0';

    return str;
}