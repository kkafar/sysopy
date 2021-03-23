#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#define MAX_NUM 32

#ifdef LIB
const char * TYPE = "LIB";
#endif

#ifdef SYS
const char * TYPE = "SYS";
#endif

int generate_random_numbers(size_t n, const char * pathname);

bool is_perfect_square(long n);

#ifdef BENCHMARK
void compute_elapsed_time(struct timespec * start, struct timespec * stop, struct timespec * elapsed);
void make_report(struct timespec * start, struct timespec * stop);
#endif 

int main(void) {
    const char * data = "dane.txt";
    const char * a_out = "a.txt";
    const char * b_out = "b.txt";
    const char * c_out = "c.txt";

    // generate_random_numbers(100, data);

    char num[MAX_NUM];
    int i = 0, even_count = 0;
    long number; 

#ifdef BENCHMARK
    struct timespec tp_start, tp_stop;
    clock_gettime(CLOCK_REALTIME, &tp_start);
#endif

#ifdef LIB
    FILE * file = fopen(data, "r");
    FILE * afile = fopen(a_out, "w");
    FILE * bfile = fopen(b_out, "w");
    FILE * cfile = fopen(c_out, "w");

    if (!file || !afile || !bfile || !cfile) {
        fprintf(stderr, "errno: %d, %s\n", errno, strerror(errno));
        fclose(file);
        fclose(afile);
        fclose(bfile);
        fclose(cfile);
        exit(1);
    }
 
    while (fread(num + i, sizeof(char), 1, file)) {
        if (num[i] == '\n') {
            num[i + 1] = 'X';
            number = strtol(num, NULL, 10);
            
            if (!(number & 1))                                      
                ++even_count;
            if (i > 2 && (num[i - 2] == '7' || num[i - 2] == '0')) { 
                fwrite(num, sizeof(char), i, bfile);
                fwrite("\n", sizeof(char), 1, bfile);
            }
            if (is_perfect_square(number)) { 
                fwrite(num, sizeof(char), i, cfile);
                fwrite("\n", sizeof(char), 1, cfile);
            }
            i = -1;
        }  
        ++i;
    }

    if (feof(file)) {
        // printf("Liczb parzystych: %d\n", even_count);
        int n = sprintf(num, "%d", even_count);
        char line[] = "Liczb parzystych jest ";
        fwrite(line, sizeof(char), 22, afile);
        fwrite(num, sizeof(char), n, afile);
        fwrite("\n", sizeof(char), 1, afile);
    } else if (ferror(file)) {
        fprintf(stderr, "errno: %d, %s\n", errno, strerror(errno));
        fclose(file);
        fclose(afile);
        fclose(bfile);
        fclose(cfile);
        exit(2);
    }

    fclose(file);
    fclose(afile);
    fclose(bfile);
    fclose(cfile);
#endif

#ifdef SYS
    int fddata = -1, fda, fdb, fdc;

    fddata  = open(data, O_RDONLY);
    fda     = open(a_out, O_WRONLY | O_CREAT, S_IRWXU);
    fdb     = open(b_out, O_WRONLY | O_CREAT, S_IRWXU);
    fdc     = open(c_out, O_WRONLY | O_CREAT, S_IRWXU);

    if (fddata == -1 || fda == -1 || fdb == -1 || fdc == -1) {
        fprintf(stderr, "errno: %d, %s\n", errno, strerror(errno));
        close(fddata);
        close(fda);
        close(fdb);
        close(fdc);
        exit(3);
    }

    int retcode;

    while ((retcode = read(fddata, num + i, sizeof(char))) > 0) {
        if (num[i] == '\n') {
            num[i + 1] = 'X';
            number = strtol(num, NULL, 10);

            if (!(number & 1)) 
                ++even_count;

            if (i > 2 && (num[i - 2] == '7' || num[i - 2] == '0')) { 
                write(fdb, num, i * sizeof(char));
                write(fdb, "\n", sizeof(char));
            }
            if (is_perfect_square(number)) { 
                write(fdc, num, sizeof(char) * i);
                write(fdc, "\n", sizeof(char));
            }
            i = -1;
        }
        ++i;
    }

    if (retcode == 0) {
        int n = sprintf(num, "%d", even_count);
        char line[] = "Liczb parzystych jest ";
        write(fda, line, 22 * sizeof(char));
        write(fda, num, n * sizeof(char));
        write(fda, "\n", sizeof(char));
    } else if (retcode == -1) {
        fprintf(stderr, "errno: %d, %s\n", errno, strerror(errno));
        close(fda);
        close(fdb);
        close(fdc);
        close(fddata);
        exit(4);
    }

    close(fda);
    close(fdb);
    close(fdc);
    close(fddata);
#endif

#ifdef BENCHMARK
    clock_gettime(CLOCK_REALTIME, &tp_stop);
    make_report(&tp_start, &tp_stop);
#endif
    
    return 0;
}


int generate_random_numbers(size_t n, const char * pathname) {
    if (n <= 0 || !pathname) return -1;

    srand(time(NULL));

    FILE * file = fopen(pathname, "w");

    if (!file) {
        fclose(file);
        fprintf(stderr, "errno: %d, %s\n", errno, strerror(errno));
        exit(3);
    }

    for (size_t i = 0; i < n; ++i)
        fprintf(file, "%d\n", rand());

    fclose(file);
    return 0;
}

bool is_perfect_square(long n) {
    long sqn = (long)(ceil(sqrt(n)));
    return sqn * sqn == n;
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

    FILE * report = fopen("pomiar_zad_3.txt", "w");

    if (!report) {
        fprintf(stderr, "%d: errno: %d, %s\n", __LINE__, errno, strerror(errno));
        fclose(report);
        exit(1);
    }

    fprintf(report, "Type: %s, realtime: %ld.%5lds\n", TYPE, elapsed.tv_sec, elapsed.tv_nsec);

    fclose(report);
}
#endif