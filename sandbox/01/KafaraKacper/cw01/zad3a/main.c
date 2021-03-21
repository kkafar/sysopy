#include "zad1.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <sys/times.h>


void clear(void) { system("clear"); }

void prompt(void) { printf("Enter command:\n$ "); }

void printmssg(const char * mssg) { printf("%s\n", mssg); }

void promptmssg(const char * mssg) { printf("%s:\n$ ", mssg); }

void clear_input_buffer(void) { while (getc(stdin) != '\n'); }

double computetime_diff(clock_t start, clock_t stop);

double computetime(clock_t elapsed);

block * fill_files(size_t nfiles, int nlines, int linewidth);

void test_merge(size_t ncases, size_t test_cases[][3], const char * report_pathname);

void test_block_save(size_t ncases, size_t test_cases[][3], const char * report_pathname);

void test_block_remove(size_t ncases, size_t test_cases[][3], const char * report_pathname);

void test_block_add_remove(size_t ncases, size_t test_cases[][3], const char * report_pathname);

void make_report(   const char * pathname, 
                    const char * test, 
                    size_t ncases, 
                    clock_t clock_ticks[], 
                    size_t test_cases[][3], 
                    struct tms tstart[], 
                    struct tms tstop[]);



int main(int argc, char * argv[]) 
{
    if (argc > 2)
    {
        if (argv[1][strlen(argv[1]) - 1] == '\n') argv[1][strlen(argv[1]) - 1] = 0;

        if (strcmp("test", argv[1]) == 0)
        {
            if ((   
                    strcmp("merge", argv[2]) == 0 || 
                    strcmp("save", argv[2]) == 0 || 
                    strcmp("remove", argv[2]) == 0 ||
                    strcmp("add_remove", argv[2]) == 0
                ) && 
                argc > 4
               )
            {
                size_t ncases = strtol(argv[3], NULL, 10);

                if (argc != 5 + ncases * 3)
                {
                    printf("Invalid number of parameters\n");
                    exit(1);
                }

                size_t test_cases[ncases][3];
                for (size_t i = 4, j = 0; i < ncases * 3 + 4; i += 3, ++j)
                {
                    test_cases[j][0] = strtol(argv[i], NULL, 10);
                    test_cases[j][1] = strtol(argv[i + 1], NULL, 10);
                    test_cases[j][2] = strtol(argv[i + 2], NULL, 10);
                }

                char * report_pathname = argv[4 + ncases * 3];

                if (strcmp("merge", argv[2]) == 0)        test_merge(ncases, test_cases, report_pathname);
                else if (strcmp("save", argv[2]) == 0)    test_block_save(ncases, test_cases, report_pathname);
                else if (strcmp("remove", argv[2]) == 0)  test_block_remove(ncases, test_cases, report_pathname);
                else                                      test_block_add_remove(ncases, test_cases, report_pathname);
            }
            else 
            {
                printf("Invalid parameter or number of parameters\n");
                exit(1);
            }
        }

        else
        {
            printf("Invalid parameter: %s or number of parameters.\n", *(argv + 1));
            exit(1);
        }
    }
    else if (argc == 2)
    {
        printf("Invalid number of parameters: %d\n", argc);
        exit(2);
    }
    else 
    {
        blockch * blkc = NULL;
        char buf[LINE_WIDTH];

        clear();
        prompt();
        while (fgets(buf, LINE_WIDTH, stdin) != NULL)
        {
            if (buf[strlen(buf) - 1] == '\n') buf[strlen(buf) - 1] = 0;

            if (strcmp("create_table", buf) == 0) 
            {
                clear();
                promptmssg("Enter number of blocks (number of pairs of files to merge)");

                size_t blkcsize;
                scanf("%ld", &blkcsize);
                clear_input_buffer();
                
                if (blkc) blockch_delete_all(blkc);

                blkc = blockch_create(blkcsize);

                if (!blkc) 
                {
                    printmssg("Failed to allocate blockchain!");
                    break;
                }
            }

            else if (strcmp("merge_files", buf) == 0)
            {
                clear();
                if (!blkc) printmssg("You must create blockchain first!");
                else
                {
                    block * fileseq = block_create(2 * blkc->size);
                    
                    if (!fileseq) 
                    {
                        printmssg("Failed to allocate file sequence!");
                        // break;
                        exit(1);
                    }

                    printf("Insert %ld files to merge!\n", 2 * blkc->size);
                    for (size_t i = 0; i < 2 * blkc->size; ++i) 
                    {
                        printf("Pair:%ld, File: %ld:$ ", i >> 1, i & 1);
                        scanf("%s", buf);
                        clear_input_buffer();
                        block_insert_at(fileseq, i, buf);
                    }

                    merge_files(fileseq, blkc, 1);
                    block_delete(fileseq);
                }
            }               

            else if (strcmp("remove_block", buf) == 0)
            {
                clear();
                if (!blkc) printmssg("You must create blockchain first!");
                else 
                {
                    size_t index; 
                    promptmssg("Insert index");
                    scanf("%ld", &index);
                    clear_input_buffer();
                    clear();
                    block_clear(blkc->blkarr + index);
                }
            }

            else if (strcmp("remove_row", buf) == 0)
            {
                clear();
                if (!blkc) printmssg("You must create blockchain first!");
                else
                {
                    size_t blk_idx, r_idx;
                    promptmssg("Insert block_index row_index");
                    scanf("%ld %ld", &blk_idx, &r_idx);
                    clear_input_buffer();
                    clear();
                    block_remove_from(blkc->blkarr + blk_idx, r_idx);
                }
            }

            else if (strcmp("print_blocks", buf) == 0)
            {
                clear();
                blockch_print(blkc);
            }

            else if (strcmp("exit", buf) == 0)
                break;


            else 
            {
                clear();
                printmssg("Unknown command");
            }

            prompt();
        }

        if (blkc) blockch_delete_all(blkc);
    }
    return 0;
}


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

block * fill_files(size_t nfiles, int nlines, int linewidth)
{
    block * fileseq = block_create(nfiles);
    FILE * f;
    char * pathname;
    char * str;


    for (size_t j = 0; j < nfiles; ++j) 
    {
        pathname = get_rand_str(15);

        f = fopen(pathname, "w");

        if (!f)
        {
            printf("Unable to open/create file %s\n", pathname);
            continue;
        }

        for (int i = 0; i < nlines; ++i)
        {
            str = get_rand_str(linewidth);
            fputs(strcat(str, "\n"), f);
            free(str);
        }

        fclose(f);
        fileseq->fline[j] = pathname;
    }

    return fileseq;
}

void test_merge(size_t ncases, size_t test_cases[][3], const char * report_pathname)
{
    blockch * blkc;
    block * fileseq;

    struct tms tstart[ncases], tstop[ncases];
    clock_t start;
    clock_t clock_ticks[ncases];


    for (size_t i = 0; i < ncases; ++i)
    {
        fileseq = fill_files(2 * test_cases[i][0], test_cases[i][2], test_cases[i][1]);

        start = times(&tstart[i]);        
        blkc = blockch_create(test_cases[i][0]);
        merge_files(fileseq, blkc, 0);
        blockch_delete_all(blkc);
        clock_ticks[i] = times(&tstop[i]) - start;

        rm_tmp_files(fileseq);
        block_delete(fileseq);
    }
    make_report(
        report_pathname,
        "MERGE",
        ncases,
        clock_ticks,
        test_cases,
        tstart,
        tstop        
    );
}

void test_block_save(size_t ncases, size_t test_cases[][3], const char * report_pathname)
{
    blockch * blkc;
    block * fileseq;
    struct tms tstart[ncases], tstop[ncases];
    clock_t start;
    clock_t clock_ticks[ncases]; 

    for (size_t i = 0; i < ncases; ++i)
    {
        // potrzebuje funkcji ładującej blok z pamięci
        fileseq = fill_files(test_cases[i][0], test_cases[i][2], test_cases[i][1]);
        
        blkc = blockch_create(test_cases[i][0]);

        for (size_t j = 0; j < test_cases[i][0]; ++j)
            blockch_read_block(blkc, fileseq->fline[j]);
    
        rm_tmp_files(fileseq);

        start = times(&tstart[i]);

        for (size_t j = 0; j < test_cases[i][0]; ++j)
        {
            block_save(blkc->blkarr + j, fileseq->fline[i]);
        }

        clock_ticks[i] = times(&tstop[i]) - start;
        
        rm_tmp_files(fileseq);
        block_delete(fileseq);
        blockch_delete_all(blkc);
    }

    make_report(
        report_pathname,
        "BLOCK SAVE",
        ncases,
        clock_ticks,
        test_cases,
        tstart,
        tstop        
    );

}

void test_block_remove(size_t ncases, size_t test_cases[][3], const char * report_pathname)
{
    blockch * blkc;
    block * fileseq;
    struct tms tstart[ncases], tstop[ncases];
    clock_t start;
    clock_t clock_ticks[ncases]; 

    for (size_t i = 0; i < ncases; ++i)
    {
        fileseq = fill_files(test_cases[i][0], test_cases[i][2], test_cases[i][1]);
        
        blkc = blockch_create(test_cases[i][0]);

        for (size_t j = 0; j < test_cases[i][0]; ++j)
            blockch_read_block(blkc, fileseq->fline[j]);
            
        rm_tmp_files(fileseq);
        block_delete(fileseq);

        start = times(&tstart[i]);
        blockch_delete_all(blkc);
        clock_ticks[i] = times(&tstop[i]) - start;
    }

    make_report(
        report_pathname,
        "BLOCK REMOVE",
        ncases,
        clock_ticks,
        test_cases,
        tstart,
        tstop
    );
}

void test_block_add_remove(size_t ncases, size_t test_cases[][3], const char * report_pathname)
{
    blockch * blkc;
    block * fileseq;
    struct tms tstart[ncases], tstop[ncases];
    clock_t start;
    clock_t clock_ticks[ncases]; 
    int noperations = 4;

    for (size_t i = 0; i < ncases; ++i)
    {
        fileseq = fill_files(test_cases[i][0], test_cases[i][2], test_cases[i][1]);

        start = times(&tstart[i]);
        for (int k = 0; k < noperations; ++k) 
        {
            blkc = blockch_create(test_cases[i][0]);

            for (size_t j = 0; j < test_cases[i][0]; ++j)
                blockch_read_block(blkc, fileseq->fline[j]);

            blockch_delete_all(blkc);
        }
        clock_ticks[i] = times(&tstop[i]) - start;

        rm_tmp_files(fileseq);
        block_delete(fileseq);
    }

    make_report(
        report_pathname, 
        "ADD/REMOVE x4",
        ncases,
        clock_ticks,
        test_cases,
        tstart,
        tstop
    );
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
        printf("case %ld: pairs: %ld, lwidth: %ld, nlines: %ld, ", i+1, test_cases[i][0], test_cases[i][1], test_cases[i][2]);
        printf("\t\trealtime: %.5lf\tusertime: %.5lf\tsystime: %.5lf\n", 
            computetime(clock_ticks[i]), 
            computetime_diff(tstart[i].tms_utime, tstop[i].tms_utime), 
            computetime_diff(tstart[i].tms_stime, tstop[i].tms_stime));
        fprintf(report, "case %ld: pairs: %ld, lwidth: %ld, nlines: %ld, ", i+1, test_cases[i][0], test_cases[i][1], test_cases[i][2]);
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