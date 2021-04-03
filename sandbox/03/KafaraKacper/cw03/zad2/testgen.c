#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include "zad1.h"

#define MAX_PATH 256

int generate_random_string(char buf[], size_t leng);

int fill_text_file(const char pathname[], size_t linewidth, size_t nolines);


int main(int argc, char * argv[]) {
    if (argc != 5) {
        fprintf(stderr, "%s: %d: Bad arg count. Expecting 5: <dirpath> <n. of files> <linewidth> <n. of lines>\n", __func__, __LINE__);
        exit(EXIT_FAILURE);
    }

    srand(time(NULL));

    size_t dirpath_len = strlen(argv[1]);
    
    if (dirpath_len > MAX_PATH || (dirpath_len + 1 >= MAX_PATH && argv[1][dirpath_len - 1] != '/')) {
        fprintf(stderr, "%s: %d: Invalid dirpath. Most likely too long.\n", __func__, __LINE__);
        exit(EXIT_FAILURE);
    }

    char dirpath[MAX_PATH];
    strcpy(dirpath, argv[1]);
    
    if (dirpath[dirpath_len - 1] != '/') {
        dirpath[dirpath_len] = '/';
        dirpath[dirpath_len + 1] = 0;
        ++dirpath_len;
    }

    size_t nofiles, linewidth, nolines;
    char filename[31];
    char filepath[MAX_PATH];
    int ret;

    nofiles = strtol(argv[2], NULL, 10);
    linewidth = strtol(argv[3], NULL, 10);
    nolines = strtol(argv[4], NULL, 10);

    for (size_t i = 0; i < nofiles; ++i) {
        if ((ret = generate_random_string(filename, 30)) != 0) {
            continue;
        }

        strcpy(filepath, dirpath);
        strcat(filepath, filename);

        fill_text_file(filepath, linewidth, nolines);
    }


    exit(EXIT_SUCCESS);
}

int generate_random_string(char buf[], size_t leng) {
    if (!buf) {
        fprintf(stderr, "%s: %d: null pointer passed.\n", __func__, __LINE__);
        return -1;
    }

    static char alphabet[] = "qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM";

    for (size_t i = 0; i < leng; ++i) buf[i] = alphabet[rand() % 52];

    buf[leng] = 0;

    return 0;
}

int fill_text_file(const char pathname[], size_t linewidth, size_t nolines) {
    if (!pathname) {
        fprintf(stderr, "%s: %d: Null pointer passed.\n", __func__, __LINE__);
        return -1;
    } else if (linewidth <= 0 || nolines <= 0) {
        fprintf(stderr, "%s: %d: Invalid argument values passed\n", __func__, __LINE__);
        return -2;
    }

    FILE * file;

    if ((file = fopen(pathname, "w")) == NULL) {
        int errnum = errno;
        fprintf(stderr, "%s: %d: %s\n", __func__, __LINE__, strerror(errnum));
        return errnum;
    }    

    char buf[LINE_WIDTH];
    int ret;

    for (size_t i = 0; i < nolines; ++i) {
        if ((ret = generate_random_string(buf, linewidth)) != 0) {
            continue;
        }
        fprintf(file, "%s\n", buf);
    }

    fclose(file);
    
    return 0;
}