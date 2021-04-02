#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <strings.h>
#include <time.h>
#include <sys/times.h>
#include <stdbool.h>
#include <dirent.h>
#include <errno.h>
#include "zad1.h"

#define MAX_PATH 256

void make_report (
    const char * pathname, 
    const char * test,
    size_t ncases,
    clock_t clock_ticks[],
    size_t test_cases[][3],
    struct tms tstart[],
    struct tms tstop[]
);

double computetime_diff(clock_t start, clock_t stop);

double computetime(clock_t elapsed);

int main(int argc, char * argv[]) {
    if (argc <= 2) {
        fprintf(stderr, "%d: Bad arg count! Expected at least 2; provided with %d\n", __LINE__, argc);
        exit(EXIT_FAILURE);
    }

    // parsing options 
    char option;
    bool test = false;
    char * test_dirpath = NULL;

    while ((option = getopt(argc, argv, ":td:")) != -1) {
        if (option == 't') {
            test = true;
        } else if (option == 'd') {                
            test_dirpath = (char *) malloc(sizeof(char) * MAX_PATH);
            strcpy(test_dirpath, optarg);

            size_t dirpath_length = strlen(test_dirpath);
            if (test_dirpath[dirpath_length - 1] != '/') {
                if (dirpath_length + 1 >= MAX_PATH) {
                    fprintf(stderr, "%d: Path length limit exceeded. (Path must be terminated with '/' sign.\n", __LINE__);
                    free(test_dirpath);
                    exit(EXIT_FAILURE);
                } 
                test_dirpath[dirpath_length] = '/';
                test_dirpath[dirpath_length + 1] = 0;
                ++dirpath_length;
            }
        } else if (option == '?') {
            fprintf(stderr, "%d: Invalid option character: %c\n", __LINE__, optopt);
        } else if (option == ':') {
            fprintf(stderr, "%d: -%c option requires argument!\n", __LINE__, 'd');
            exit(EXIT_FAILURE);
        }
    }

    if (test) {
        if (!test_dirpath) {
            fprintf(stderr, "%d: test directory not specified!\n", __LINE__);
            exit(EXIT_FAILURE);
        }
        
        DIR * test_dir; 

        if ((test_dir = opendir(test_dirpath)) == NULL) {
            int errnum = errno;
            fprintf(stderr, "%d: %s\n", __LINE__, strerror(errnum));
            exit(errnum);
        }

        struct dirent * dirp;            
        size_t nofiles = 0;

        // najpierw zliczamy ilość wpisów w katalogu z plikami testowymi 
        // założenie jest takie, że w katalogu znajdują się tylko i wyłącznie pliki testowe
        // oraz jest ich odpowiednia ilość
        while ((dirp = readdir(test_dir)) != NULL) ++nofiles;

        if ((nofiles & 1)) {
            fprintf(stderr, "%d: Odd number of files in test files catalogue.\n", __LINE__);
            closedir(test_dir);
            free(test_dirpath);
            exit(EXIT_FAILURE);
        }

        rewinddir(test_dir);

        char file1[MAX_PATH], file2[MAX_PATH];

        while ((dirp = readdir(test_dir)) != NULL) {                

        }


        closedir(test_dir);
    }



    if (test_dirpath) free(test_dirpath);

    exit(EXIT_SUCCESS);
}

void make_report(   const char * pathname, 
                    const char * test, 
                    size_t ncases, 
                    clock_t clock_ticks[], 
                    size_t test_cases[][3], 
                    struct tms tstart[], 
                    struct tms tstop[])
{
    FILE * report = fopen(pathname, "w");
    if (!report)
    {
        printf("Could not create/open the file %s for write!\n", pathname);
        return;
    }
    printf("==================== %s REPORT ======================\n", test);
    fprintf(report, "====================  %s REPORT ======================\n", test);
    for (size_t i = 0; i < ncases; ++i)
    {
        printf("case %3ld: pairs: %5ld, lwidth: %5ld, nlines: %6ld, ", i+1, test_cases[i][0], test_cases[i][1], test_cases[i][2]);
        printf("\t\trealtime: %.5lf\tusertime: %.5lf\tsystime: %.5lf\n", 
            computetime(clock_ticks[i]), 
            computetime_diff(tstart[i].tms_utime, tstop[i].tms_utime), 
            computetime_diff(tstart[i].tms_stime, tstop[i].tms_stime));
        fprintf(report, "case %3ld: pairs: %5ld, lwidth: %5ld, nlines: %6ld, ", i+1, test_cases[i][0], test_cases[i][1], test_cases[i][2]);
        fprintf(report, "\t\trealtime: %.5lf\tusertime: %.5lf\tsystime: %.5lf\n", 
            computetime(clock_ticks[i]), 
            computetime_diff(tstart[i].tms_utime, tstop[i].tms_utime), 
            computetime_diff(tstart[i].tms_stime, tstop[i].tms_stime));
    }
    fprintf(report, "================== END %s REPORT =====================\n", test);
    printf("================== END %s REPORT =====================\n", test);
    fclose(report);
}

double computetime_diff(clock_t start, clock_t stop) 
{
    return (double) (stop - start) / sysconf(_SC_CLK_TCK);
}

double computetime(clock_t elapsed)
{
    return (double) elapsed / sysconf(_SC_CLK_TCK);
}