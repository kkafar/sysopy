#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#define MAX_LINE 8192
#define PRODUCERS 5

int generate_test_data(const char pathname[], char ch, int length);
void generate_string(char ch, int length, char buf[]);

int main(int argc, char * argv[])
{
    if (argc != PRODUCERS + 1) err("bad arg count", __FILE__, __func__, __LINE__);

    const char * const fifo_path = "./fifo";
    if (mkfifo(fifo_path, 0666) < 0)
    {
        if (errno == EEXIST) 
        {
            err_noexit("./fifo file already exists, replacing it with new ./fifo", __FILE__, __func__, __LINE__);
            if (remove(fifo_path) < 0 || mkfifo(fifo_path, 0666) < 0)
                syserr("./fifo already exists and failed to replace it. Remove it before executing this program.", __FILE__, __func__, __LINE__);
        }
        else
            syserr("mkfifo error", __FILE__, __func__, __LINE__);
    }

    if (generate_test_data("./testdata/input1.txt", 'A', strtol(argv[1], NULL, 10)) < 0) 
        err("failed to generate input file for 1st producent; make sure ./testdata folder exists", __FILE__, __func__, __LINE__);
    if (generate_test_data("./testdata/input2.txt", 'B', strtol(argv[2], NULL, 10)) < 0)
        err("failed to generate input file for 2nd producent; make sure ./testdata folder exists", __FILE__, __func__, __LINE__);
    if (generate_test_data("./testdata/input3.txt", 'C', strtol(argv[3], NULL, 10)) < 0)
        err("failed to generate input file for 3rd producent; make sure ./testdata folder exists", __FILE__, __func__, __LINE__);
    if (generate_test_data("./testdata/input4.txt", 'D', strtol(argv[4], NULL, 10)) < 0)
        err("failed to generate input file for 4th producent; make sure ./testdata folder exists", __FILE__, __func__, __LINE__);
    if (generate_test_data("./testdata/input5.txt", 'E', strtol(argv[5], NULL, 10)) < 0)
        err("failed to generate input file for 5th producent; make sure ./testdata folder exists", __FILE__, __func__, __LINE__);

    const char * const consumer_executable = "./konsument ./fifo ./testdata/testresult.txt 4";
    // const int producer_ids[] = {1, 3, 4, 5, 9};
    const char * const producers[] = {
        "./producent ./fifo 0 ./testdata/input1.txt 4",
        "./producent ./fifo 3 ./testdata/input2.txt 4",
        "./producent ./fifo 4 ./testdata/input3.txt 4",
        "./producent ./fifo 5 ./testdata/input4.txt 4",
        "./producent ./fifo 9 ./testdata/input5.txt 4",
    };


    /* launching consumer process */
    FILE * consumer_file;
    if ((consumer_file = popen(consumer_executable, "w")) == NULL) syserr("failed to launch consumer process", __FILE__, __func__, __LINE__);

    FILE * producents_file[PRODUCERS];
    for (int i = 0; i < PRODUCERS; ++i)
        if ((producents_file[i] = popen(producers[i], "w")) == NULL) syserr_noexit("failed to launch producent process", __FILE__, __func__, __LINE__);

    for (int i = 0; i < PRODUCERS; ++i)
        if (pclose(producents_file[i]) < 0) syserr("pclose failed", __FILE__, __func__, __LINE__);

    pclose(consumer_file);
    return 0;
}

int generate_test_data(const char pathname[], char ch, int length)
{
    FILE * file;
    if ((file = fopen(pathname, "w")) == NULL) return -1;

    char buf[MAX_LINE];
    int retcode;
    generate_string(ch, length, buf);

    if ((retcode = fwrite(buf, sizeof(char), length, file)) <= 0) return -1;

    fclose(file);
    return 0;
}

void generate_string(char ch, int length, char buf[])
{
    for (int i = 0; i < length; ++i)
        buf[i] = ch;
    buf[length] = 0;
}