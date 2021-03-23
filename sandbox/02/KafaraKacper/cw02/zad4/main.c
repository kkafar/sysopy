#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>


int main(int argc, char * argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Invalid number of arguments: %d. Expectd 4.\n", argc);
        exit(1);
    }

    const char * input_file     = argv[1];
    const char * output_file    = argv[2];
    const char * str1           = argv[3];
    const char * str2           = argv[4];



    return 0;
}