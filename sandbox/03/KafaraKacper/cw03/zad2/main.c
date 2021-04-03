#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <strings.h>
#include <time.h>
#include <sys/times.h>
#include <stdbool.h>
#include <dirent.h>
#include <errno.h>
#include <sys/wait.h>
#include "zad1.h"

#define MAX_PATH 256

void make_report(const char * pathname, clock_t realtime, struct tms * start, struct tms * stop);

double computetime_diff(clock_t start, clock_t stop);

double computetime(clock_t elapsed);

int main(int argc, char * argv[]) {
    if (argc <= 2) {
        fprintf(stderr, "%d: Bad arg count! Expected at least 2; provided with %d\n", __LINE__, argc - 1);
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

        // odejmujemy . oraz ..
        nofiles -= 2;

        if ((nofiles & 1)) {
            fprintf(stderr, "%d: Odd number of files in test files catalogue.\n", __LINE__);
            closedir(test_dir);
            free(test_dirpath);
            exit(EXIT_FAILURE);
        }

        rewinddir(test_dir);

        char filepath[MAX_PATH];
            

        struct tms start, stop;
        clock_t time; 
        block * fseq[nofiles / 2];

        for (size_t i = 0; i < nofiles / 2; ++i) 
            fseq[i] = block_create(2);

        size_t i = 0;

        while ((dirp = readdir(test_dir)) != NULL) {    
            if (strcmp(".", dirp->d_name) == 0 || strcmp("..", dirp->d_name) == 0) 
                continue;
            strcpy(filepath, test_dirpath);
            strcat(filepath, dirp->d_name);
            block_insert_at(fseq[i / 2], i & 1, filepath);
            ++i;
        }
        closedir(test_dir);

        // Mergowanie
        pid_t cpid;

        time = times(&start);
        for (size_t i = 0; i < nofiles / 2; ++i) {                
            if ((cpid = fork()) == 0) {
                blockch * blkc = blockch_create(1);

                merge_files(fseq[i], blkc, 0, NULL);

                blockch_delete_all(blkc);
                for (size_t i = 0; i < nofiles / 2; ++i) block_delete(fseq[i]);
                free(test_dirpath);

                exit(EXIT_SUCCESS);
            } else if (cpid == -1) {
                fprintf(stderr, "%s: %d: Could not merge %s %s\n", __func__, __LINE__, fseq[i]->fline[0], fseq[i]->fline[1]);
            }
        }

        // czekamy na wszystkie dzieci 
        while (waitpid(-1, NULL, 0) != -1);

        time = times(&stop) - time;

        for (size_t i = 0; i < nofiles / 2; ++i) block_delete(fseq[i]);

        make_report("./report.txt", time, &start, &stop);
    } else {
        // jeżeli nie prowadzimy testów to należy wprowadzić parzystą liczbę plików do mergowania
        // format: <plik A> <plik B> <plik wynikowy> ...

        size_t argcount = argc - optind;
        if (argcount % 3 != 0) {
            fprintf(stderr, "%s: %d: Bad arg count.\n", __func__, __LINE__);
            if (test_dirpath) free(test_dirpath);
            exit(EXIT_FAILURE);
        } 

        blockch * blkc = blockch_create(argcount / 3);
        block * fseq = block_create(argcount / 3 * 2);
        block * savefseq = block_create(argcount / 3);

        for (size_t i = 0, j = 0; optind < argc; optind += 3, ++i, j += 2) {
            block_insert_at(fseq, j, argv[optind]);
            block_insert_at(fseq, j + 1, argv[optind + 1]);
            block_insert_at(savefseq, i, argv[optind + 2]);
        }

        block_print(fseq, 0);
        block_print(savefseq, 0);

        merge_files(fseq, blkc, 1, savefseq);

        block_delete(fseq);
        block_delete(savefseq);
        blockch_delete_all(blkc);
    }

    if (test_dirpath) free(test_dirpath);

    exit(EXIT_SUCCESS);
}

void make_report(const char * pathname, clock_t realtime, struct tms * start, struct tms * stop) {
    FILE * report = fopen(pathname, "w");
    if (!report) {
        fprintf(stderr, "Could not create/open the file %s for write!\n", pathname);
        return;
    }

    fprintf(stdout, "realtime: %.5lf, usertime: %.5f, systime: %.5f, cusertime: %.5f, csystime: %.5f\n", 
        computetime(realtime), 
        computetime_diff(stop->tms_utime, start->tms_utime),
        computetime_diff(stop->tms_stime, start->tms_stime),
        computetime_diff(stop->tms_cutime, start->tms_cutime),
        computetime_diff(stop->tms_cstime, start->tms_cstime));

    fprintf(report, "realtime: %.5lf, usertime: %.5f, systime: %.5f, cusertime: %.5f, csystime: %.5f\n", 
        computetime(realtime), 
        computetime_diff(stop->tms_utime, start->tms_utime),
        computetime_diff(stop->tms_stime, start->tms_stime),
        computetime_diff(stop->tms_cutime, start->tms_cutime),
        computetime_diff(stop->tms_cstime, start->tms_cstime));

    fclose(report);
}

double computetime_diff(clock_t stop, clock_t start) {
    return (double) (stop - start) / sysconf(_SC_CLK_TCK);
}

double computetime(clock_t elapsed) {
    return (double) elapsed / sysconf(_SC_CLK_TCK);
}